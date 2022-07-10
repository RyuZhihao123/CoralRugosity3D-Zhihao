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

    void FindVerticalPlane(QVector2D startPt2D, QVector2D endPt2D);

    // 更新/绘制Curve的对象
    void UpdateCurrentMesh();
    void RenderCurrentMesh(QOpenGLShaderProgram *&program, const QMatrix4x4 &modelMat);

    // interfaces
    Object& GetInputMesh() {return this->m_inputMesh;}

};

#endif // RUGOSITYALGORITHM_H
