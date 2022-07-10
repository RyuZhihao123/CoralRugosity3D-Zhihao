#include "rugosityalgorithm.h"

RugosityAlgorithm::RugosityAlgorithm()
{

}

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
