#ifndef RUGOSITYALGORITHM_H
#define RUGOSITYALGORITHM_H
#include "object.h"
#include "3rdparty/kdtree.h"



class RugosityAlgorithm
{
public:
    RugosityAlgorithm();

    QVector<Curve> m_curves;
    QVector<Object> m_curveObjects;


    Object m_inputMesh;

    // kd树相关
    std::vector<simplePoint3> cloud;  // marker Points in simplePoint3
    KDTree<simplePoint3> index;       // kd tree

    void BuildKDTree();
    std::vector<size_t> RadiusSearch(const QVector3D &pos, float radius);
    std::vector<size_t> CubeSearch(const QVector3D &pos, float radiusSize);

    QVector<OctreenNode> m_octree;
    void BuildPartition_WithOctree(float splitCount);
    
    QVector<int> QueryIntersectedOctreeBoxIDs(Ray mouseray);
    bool QueryIntersectedPointAmong(Ray ray, QVector<int> seletedIDs, QVector3D& outIntersectionPt);

    QVector<QVector3D> GetRugosityCurvePoints(QVector3D intersectPtA, QVector3D intersectPtB, QVector3D normal);

    // 寻找平面算法
    void FindVerticalPlane(QVector2D startPt2D, QVector2D endPt2D);

    // 更新/绘制Curve的对象
    void UpdateCurrentMesh();
    void RenderCurrentMesh(QOpenGLShaderProgram *&program, const QMatrix4x4 &modelMat);
    void RenderAllOctreeNode(QOpenGLShaderProgram *&program);
    void RenderSelectedOctreeNode(QOpenGLShaderProgram *&program, const QVector<int>& selectedNodeId);

    // interfaces
    Object& GetInputMesh() {return this->m_inputMesh;}

};

#endif // RUGOSITYALGORITHM_H
