#include "rugosityalgorithm.h"
#include <tuple>
RugosityAlgorithm::RugosityAlgorithm()
{

}


//////
/// KD-Tree 相关函数
//////
void RugosityAlgorithm::BuildKDTree()
{
    for(unsigned int i=0; i<m_inputMesh.m_vertices.size(); ++i)
        cloud.push_back(GlobalTools::GetSimplePoint(m_inputMesh.m_vertices[i]));

    index.buildTree(cloud);
}

std::vector<size_t> RugosityAlgorithm::RadiusSearch(const QVector3D &pos, float radius)
{
    auto closeNodes = index.getPointsWithinCube(GlobalTools::GetSimplePoint(pos),
                                                radius);
    std::vector<size_t> res;
    for(unsigned int i=0; i<closeNodes.size(); ++i)
    {
        QVector3D pt = GlobalTools::GetVector3D(cloud[closeNodes[i]]);

        if(pt.distanceToPoint(pos) < radius)
            res.push_back(closeNodes[i]);
    }
    return res;
}

std::vector<size_t> RugosityAlgorithm::CubeSearch(const QVector3D &pos, float radiusSize)
{
    auto closeNodes = index.getPointsWithinCube(GlobalTools::GetSimplePoint(pos),radiusSize);

    return closeNodes;
}


///
/// 构建八叉树
///
struct LabelID
{
    int x, y, z;

    bool operator<( const LabelID& other) const { return std::tie(x,y,z) < std::tie(other.x,other.y,other.z);}

    static int GetBlockID(float coordinate, float width){

        int blockID = coordinate/width;
        if(blockID %2 == 0)  return blockID/2;
        if(blockID %2 != 0)
        {
            if(blockID > 0) return blockID/2+1;
            if(blockID < 0) return blockID/2-1;
        }
        return 0;
    }
};


void RugosityAlgorithm::BuildPartition_WithOctree(float width)
{
    QMap<LabelID, OctreenNode> octNodes;
    for(int i=0; i<m_inputMesh.m_vertices.size(); ++i)
    {
        QVector3D pt = m_inputMesh.m_vertices[i];

        LabelID id;

        id.x = LabelID::GetBlockID(pt.x(), width);
        id.y = LabelID::GetBlockID(pt.y(), width);
        id.z = LabelID::GetBlockID(pt.z(), width);

        OctreenNode& curOctNode = octNodes[id];

        if(curOctNode.vertexIDs.size() == 0)  // 如果之前没有这个octNode
        {
            curOctNode.center = QVector3D(id.x*(2*width),id.y*(2*width),id.z*(2*width));
            curOctNode.width = width;
        }
        curOctNode.vertexIDs.append(i);
    }

    for(int i=0; i<octNodes.keys().size(); ++i)
    {
        auto key = octNodes.keys()[i];
        this->m_octree.append(octNodes[key]);
    }

    qDebug()<<"OctTree构建完毕:"<<octNodes.keys().size();
}

QVector<int> RugosityAlgorithm::QueryIntersectedOctreeBoxIDs(Ray mouseray)
{
    QVector<int> intersected_octreenodes;
    QVector3D rayOrigin = mouseray.center;
    QVector3D rayDirection = mouseray.dir.normalized();

    for(int i=0; i<m_octree.size(); ++i)
    {
        if(GlobalTools::intersectAABB(rayOrigin, rayDirection, m_octree[i]))
        {
            intersected_octreenodes.append(i);
        }
    }

    return intersected_octreenodes;
}

bool RugosityAlgorithm::QueryIntersectedPointAmong(Ray ray, QVector<int> seletedIDs, QVector3D &outIntersectionPt)
{
    float minDist = 1e10;

    // 首先按照距离排序
    for(int i=0; i<seletedIDs.size(); ++i) // 当前排序的第i个位置
    {
        float minDist = 1e10;
        float minID = -1;
        for(int m=i; m<seletedIDs.size(); ++m) // 向后遍历第m个位置
        {
            QVector3D curPt = m_octree[seletedIDs[m]].center;
            float curDist = curPt.distanceToPoint(ray.center);

            if(curDist < minDist)
            {
                minID = m;
                minDist = curDist;
            }
        }

        int tmp = seletedIDs[i];
        seletedIDs[i] = seletedIDs[minID];
        seletedIDs[minID] = tmp;
    }

    // 打印一下排序后的距离
    QString sorted = "Sorted Dists:";

    for(int i=0; i<seletedIDs.size();++i)
    {
        QVector3D curPt = m_octree[seletedIDs[i]].center;
        float curDist = curPt.distanceToPoint(ray.center);

        sorted += QString::number(curDist)+" ";
    }
    //qDebug()<<sorted;

    QVector3D bestIntersectionPoint;  // 最佳的相交点
    bool isFindIntersection = false;
    minDist = 1e10;

    // 寻找相交
    for(int tt=0; tt<seletedIDs.size(); ++tt)  // 首先遍历第tt个OctreeNode-Box
    {
        int id = seletedIDs[tt];

        if(isFindIntersection)  // 如果已经找到了交点，不需要遍历后续的Box，因为已经是最近的了
            break;

        // 检测射线和该box的三角形面片相交
        OctreenNode& box = m_octree[id];
        for(int mm=0; mm<box.vertexIDs.size(); ++mm)  // 遍历该box的所有的顶点
        {
            int vid = box.vertexIDs[mm]; // 当前顶点vid
            QVector<int>& faceDict =m_inputMesh.m_dictPtToFace[vid];
            for(int i=0; i<faceDict.size(); ++i)  // 遍历该顶点vid对应的所有的面
            {
                int faceID = faceDict[i];
                uint3 triangle = m_inputMesh.m_faceids[faceID];

                QVector3D v0 = m_inputMesh.m_vertices[triangle.x];
                QVector3D v1 = m_inputMesh.m_vertices[triangle.y];
                QVector3D v2 = m_inputMesh.m_vertices[triangle.z];

                QVector3D intersectionPoint;

                //qDebug()<<ray.center<<ray.dir<<v0<<v1<<v2;


                // 如果不存在交点
                //GlobalTools::GetIntersectPoint_ray_to_triangle(ray,v0,v1,v2,intersectionPoint);
                if(!GlobalTools::GetIntersectPoint_ray_to_triangle(ray,v0,v1,v2,intersectionPoint))
                    continue;
                qDebug()<<"box:"<<id<<"vid:"<<mm<<"fid:"<<i;
                // 如果存在交点
                isFindIntersection = true;

                float curDist = intersectionPoint.distanceToPoint(ray.center);

                if(curDist < minDist)
                {
                    minDist = curDist;
                    bestIntersectionPoint = intersectionPoint;
                }
            }
        }
    }

    outIntersectionPt = bestIntersectionPoint;

    return isFindIntersection;
}


QVector<QVector3D> RugosityAlgorithm::GetRugosityCurvePoints(QVector3D intersectPtA, QVector3D intersectPtB, QVector3D normal)
{
    normal = normal.normalized();
    QVector3D p = intersectPtA;

    QVector<QVector3D> res;

    for(int tt=0; tt<this->m_octree.size(); ++tt)  // 首先遍历第tt个OctreeNode-Box
    {
        OctreenNode& box = m_octree[tt];

        float aa = box.width*box.width + box.width*box.width;
        float boxRadius = sqrt(aa+box.width*box.width);

        float distBoxToPlane = box.center.distanceToPlane(p, normal);

        if(distBoxToPlane > boxRadius)  // 如果Box距离平面太远
            continue;

        // 检测该Box的每个三角形的边和给定Plane的交点
        for(int mm=0; mm<box.vertexIDs.size(); ++mm)  // 遍历该box的所有的顶点
        {
            int vid = box.vertexIDs[mm]; // 当前顶点vid
            QVector<int>& faceDict =m_inputMesh.m_dictPtToFace[vid];

            for(int i=0; i<faceDict.size(); ++i)  // 遍历该顶点vid对应的所有的面
            {
                int faceID = faceDict[i];
                uint3 triangle = m_inputMesh.m_faceids[faceID];

                QVector3D v0 = m_inputMesh.m_vertices[triangle.x];
                QVector3D v1 = m_inputMesh.m_vertices[triangle.y];
                QVector3D v2 = m_inputMesh.m_vertices[triangle.z];
                QVector3D out0,out1,out2;

                if(GlobalTools::GetIntersectPoint_segment_to_triangle(normal,p, v0, v1,out0))
                    res.append(out0);

                if(GlobalTools::GetIntersectPoint_segment_to_triangle(normal,p, v0, v2,out1))
                    res.append(out1);

                if(GlobalTools::GetIntersectPoint_segment_to_triangle(normal,p, v1, v2,out2))
                    res.append(out2);
            }

        }
    }

    return res;
}

void RugosityAlgorithm::FindVerticalPlane(QVector2D startPt2D, QVector2D endPt2D)
{

}

void RugosityAlgorithm::UpdateCurrentMesh()
{
    for(int i=0; i<m_curveObjects.size(); ++i)
        m_curveObjects[i].ResetAll();

    m_curveObjects.clear();

    for(int i=0; i<m_curves.size(); ++i)
    {

        Object obj;
        obj.CreateLineVBO(m_curves[i]);

        m_curveObjects.append(obj);
    }
}

void RugosityAlgorithm::RenderCurrentMesh(QOpenGLShaderProgram *&program, const QMatrix4x4 &modelMat)
{
    for(int i=0; i<m_curves.size(); ++i)
    {
        m_curveObjects[i].DrawLineVBO(program, modelMat);
    }
}

void RugosityAlgorithm::RenderAllOctreeNode(QOpenGLShaderProgram *&program)
{
    for(int i=0; i<m_octree.size(); ++i)
    {
        GlobalTools::DrawAxis(program, m_octree[i].center,m_octree[i].width);
    }
}

VOID RugosityAlgorithm::RenderSelectedOctreeNode(QOpenGLShaderProgram *&program, const QVector<int> &selectedNodeId)
{
    for(int i=0; i<selectedNodeId.size(); ++i)
    {
        int id = selectedNodeId[i];
        GlobalTools::DrawAxis(program, m_octree[id].center,m_octree[id].width);
    }
}

