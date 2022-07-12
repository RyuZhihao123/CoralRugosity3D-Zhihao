#ifndef OBJECT_H
#define OBJECT_H
#include <QVector>
#include <QStack>
#include <QVector3D>
#include <cmath>
#include <QPoint>
#include <QImage>
#include <QDebug>
#include <QColor>
#include <QList>
#include <QTime>
#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QQueue>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include "3rdparty/happly.h"
#include "3rdparty/tinyply.h"
#include "3rdparty/utils.h"

struct GLBuffer
{
    QOpenGLBuffer vbo;
    int count;
};


class Object
{
public:
    Object();

    // Attributes
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_normals;
    QVector<QVector2D> m_texcoords;
    QVector<QVector3D> m_colors;
    QVector<uint3> m_faceids;

    QMap<int, QVector<int>> m_dictPtToFace; // 存储顶点对应的面片的id (vid - fid)

    // OpenGL Buffers
    GLBuffer m_meshVBO;
    QOpenGLTexture* m_texture = nullptr;

    // 八叉树及查询

    OctreenNode octreeWhole;
    QVector<OctreenNode> octreeList;
    void BuildOctree();
    QVector3D QueryPointFromRay(QVector3D camera, QVector3D cameraDir);

    // 构建vid和fid的字典
    void BuildDictionary();
    // Mesh相关
    void SetTextures(const QString& filepath);
    void UpdateMeshs();

    void CreateMeshVBO(const std::string& filepath);
    void DrawMeshVBO(QOpenGLShaderProgram *&program, const QMatrix4x4 &modelMat);

    void CreateLineVBO(const Curve& curve);
    void DrawLineVBO(QOpenGLShaderProgram *&program, const QMatrix4x4 &modelMat);


    bool hasColor() { return (m_colors.size() > 0); }
    bool hasTexture() {
        //qDebug()<<(m_texture!=nullptr)<<(m_texcoords.size()>0);
        return (m_texture!=nullptr && m_texcoords.size()>0);}

    void ResetAll();

    void InverseY_Axis(); // Y轴颠倒
    void FlipTexcoords(); // 镜像翻转纹理坐标

private:

};

#endif // OBJECT_H
