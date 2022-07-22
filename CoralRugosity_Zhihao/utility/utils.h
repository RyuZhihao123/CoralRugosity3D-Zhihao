#ifndef UTILS_H
#define UTILS_H
#include <QString>
#include <QFileInfo>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QPointF>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QTime>


struct float2 { float x, y; };
struct float3 { float x, y, z; };
struct float6 { float u1, v1, u2, v2, u3, v3;};
struct double3 { double x, y, z; };
struct uint3 { uint32_t x, y, z; };
struct uint4 { uint32_t x, y, z, w; };

struct simplePoint3
{
    double x,y,z;
    const static int dimension = 3;
    double getDimension(int dim) const
    {
        switch(dim)
        {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: return std::numeric_limits<double>::quiet_NaN();
        }
    }
};


struct Curve
{
    QVector<QVector3D> pts;
};


struct Ray
{
    QVector3D center;
    QVector3D dir;
};

class OctreenNode
{
public:
    QVector3D center;

    float width;  // width = Radius (instead of Diameter)

    QVector<size_t> vertexIDs;  // 所属的顶点ID
};

struct BoundingBox
{
    float x,y,z;
};



class GlobalTools
{
public:

    // 检查与ply文件路径的baseName相同的纹理是否存在？
    static QString checkDefaultTextureExists(const QString& plyFilePath)
    {
        QFileInfo l_info(plyFilePath);

        QString imagePath = l_info.absolutePath() + "/" + l_info.baseName() +"." + "png";
        if(QFileInfo(imagePath).exists()) return imagePath;
        imagePath = l_info.absolutePath() + "/" + l_info.baseName() +"." + "PNG";
        if(QFileInfo(imagePath).exists()) return imagePath;
        imagePath = l_info.absolutePath() + "/" + l_info.baseName() +"." + "jpg";
        if(QFileInfo(imagePath).exists()) return imagePath;
        imagePath = l_info.absolutePath() + "/" + l_info.baseName() +"." + "JPG";
        if(QFileInfo(imagePath).exists()) return imagePath;
        imagePath = l_info.absolutePath() + "/" + l_info.baseName() +"." + "bmp";
        if(QFileInfo(imagePath).exists()) return imagePath;
        imagePath = l_info.absolutePath() + "/" + l_info.baseName() +"." + "BMP";
        if(QFileInfo(imagePath).exists()) return imagePath;

        return "";
    }

    // SimplePoint3和Vector3D的转换
    static simplePoint3 GetSimplePoint(const QVector3D &pos)
    {
        simplePoint3 query_pt;
        query_pt.x = pos.x();
        query_pt.y = pos.y();
        query_pt.z = pos.z();

        return query_pt;
    }

    static QVector3D GetVector3D(const simplePoint3 &pos)
    {
        return QVector3D(pos.x,pos.y,pos.z);
    }


    static QVector3D GetRayDirection_From2DScreenPos(QPointF screen_pos, float width, float height,
                                                  QMatrix4x4 projection_matrix, QMatrix4x4 view_matrix)
    {
        float mouse_x = screen_pos.x(), mouse_y = screen_pos.y();

        float x = (2.0f * mouse_x) / width - 1.0f;
        float y = 1.0f - (2.0f * mouse_y) / height;
        float z = 1.0f;
        QVector3D ray_nds = QVector3D(x, y, z);

        QVector4D ray_clip = QVector4D(ray_nds.x(), ray_nds.y(), -1.0, 1.0);

        QVector4D ray_eye = projection_matrix.inverted() * ray_clip;

        ray_eye = QVector4D(ray_eye.x(), ray_eye.y(), -1.0, 0.0);

        QVector3D ray_wor = QVector3D(view_matrix.inverted() * ray_eye);
        // don't forget to normalise the vector at some point
        ray_wor = ray_wor.normalized();

        return ray_wor;
    }

    static bool GetIntersectPoint_ray_to_triangle(Ray ray,
                                                  QVector3D vertex0, QVector3D vertex1, QVector3D vertex2,
                                                  QVector3D& outIntersectionPoint)
    {
        QVector3D rayOrigin = ray.center;
        QVector3D rayVector = ray.dir;

        const float EPSILON = 0.0000001;

        QVector3D edge1, edge2, h, s, q;
        float a,f,u,v;
        edge1 = vertex1 - vertex0;
        edge2 = vertex2 - vertex0;
        h = QVector3D::crossProduct(rayVector, edge2);
        a = QVector3D::dotProduct(edge1, h);
        if (a > -EPSILON && a < EPSILON)
            return false;    // This ray is parallel to this triangle.
        f = 1.0/a;
        s = rayOrigin - vertex0;
        u = f * QVector3D::dotProduct(s, h);
        if (u < 0.0 || u > 1.0)
            return false;
        q = QVector3D::crossProduct(s, edge1);
        v = f * QVector3D::dotProduct(rayVector, q);
        if (v < 0.0 || u + v > 1.0)
            return false;
        // At this stage we can compute t to find out where the intersection point is on the line.
        float t = f * QVector3D::dotProduct(edge2, q);
        if (t > EPSILON) // ray intersection
        {
            outIntersectionPoint = rayOrigin + rayVector * t;
            return true;
        }
        else // This means that there is a line intersection but not a ray intersection.
            return false;
    }


    static bool GetIntersectPoint_segment_to_triangle(QVector3D n, QVector3D p, QVector3D v1, QVector3D v2, QVector3D& out)
    {
        // Rwference Link: https://math.stackexchange.com/questions/47594/plane-intersecting-line-segment

        // 注意n是单位化的向量
        float dist1 = QVector3D::dotProduct(n, v1-p);
        float dist2 = QVector3D::dotProduct(n, v2-p);

        if(dist1*dist2 >0) // 同向，
            return false;

        if(dist1==0 && dist2==0)
        {
            out = (v2+v1)/2.0f;
            return true;
        }

        if(dist1==0)
        {
            out = v1;
            return true;
        }
        if(dist2==0)
        {
            out = v2;
            return true;
        }

        QVector3D x = (v2-v1).normalized();

        float costheta = QVector3D::dotProduct(n,x);

        if(costheta==0)
        {
            out = (v2+v1)/2.0f;
            return true;
        }

        out = v2 - (dist2/costheta)*x;

        return true;
    }

//    static QVector3D GetRayFromScreenPos(QPointF screenPos, QVector3D direction, QVector3D up, float angle, float w, float h)
//    {

//        float x_uv = screenPos.x(), y_uv = screenPos.y();

//        x_uv = 2.0f * x_uv / (w - 1) - 1.0;
//        y_uv = 2.0f * y_uv / (h - 1) - 1.0;

//        QVector3D u = QVector3D::crossProduct(direction, up);
//        u = u.normalized();
//        QVector3D v = QVector3D::crossProduct(u, direction);
//        v = v.normalized();

//        QVector3D raydir = x_uv * v + y_uv * u + 1 / tan(angle) * direction;
//        raydir.normalize();

//        return raydir;
//    }

    static float max(float a, float b)
    {
        return (a>b)? a:b;
    }
    static float min(float a, float b)
    {
        return (a<b)? a:b;
    }

    static QVector3D max(QVector3D a, QVector3D b)
    {
        return QVector3D(max(a.x(),b.x()), max(a.y(),b.y()), max(a.z(),b.z()));
    }

    static QVector3D min(QVector3D a, QVector3D b)
    {
        return QVector3D(min(a.x(),b.x()), min(a.y(),b.y()), min(a.z(),b.z()));
    }


//    static bool intersectAABB(QVector3D rayOrigin, QVector3D rayDir, OctreenNode octreeBox) {

//        QVector3D boxMin, boxMax;

//        boxMin = octreeBox.center - QVector3D(octreeBox.width,octreeBox.width,octreeBox.width);
//        boxMax = octreeBox.center + QVector3D(octreeBox.width,octreeBox.width,octreeBox.width);

//        QVector3D tMin = (boxMin - rayOrigin) / rayDir;
//        QVector3D tMax = (boxMax - rayOrigin) / rayDir;
//        QVector3D t1 = min(tMin, tMax);
//        QVector3D t2 = max(tMin, tMax);

//        float tNear = max(max(t1.x(), t1.y()), t1.z());
//        float tFar = min(min(t2.x(), t2.y()), t2.z());

//        if(tNear<=tFar) return true;

//        return false;
//    }

    static bool intersectAABB(QVector3D rayOrigin, QVector3D rayDir, OctreenNode octreeBox)
    {
        double tmin = -INFINITY, tmax = INFINITY;
        QVector3D _boxMin, _boxMax;
        _boxMin = octreeBox.center - QVector3D(octreeBox.width,octreeBox.width,octreeBox.width);
        _boxMax = octreeBox.center + QVector3D(octreeBox.width,octreeBox.width,octreeBox.width);

        double boxmin[3] = {_boxMin.x(),_boxMin.y(),_boxMin.z()};
        double boxmax[3] = {_boxMax.x(),_boxMax.y(),_boxMax.z()};
        double rorigin[3] = {rayOrigin.x(),rayOrigin.y(),rayOrigin.z()};
        double rdir[3] = {rayDir.x(),rayDir.y(),rayDir.z()};

        for (int i = 0; i < 3; ++i)
        {
            if (rdir[i] != 0.0) {
                double t1 = (boxmin[i] - rorigin[i])/rdir[i];
                double t2 = (boxmax[i] - rorigin[i])/rdir[i];

                tmin = max(tmin, min(t1, t2));
                tmax = min(tmax, max(t1, t2));
            } else if (rorigin[i] < boxmin[i] || rorigin[i] > boxmax[i]) {
                return false;
            }
        }

        return tmax >= tmin && tmax >= 0.0;
    }

//     static bool intersectAABB(QVector3D rayOrigin, QVector3D rayDir, OctreenNode octreeBox)
//     {
//         // TODO: 需要重新写这个函数
//         float a_squared = octreeBox.width*octreeBox.width + octreeBox.width*octreeBox.width;
//         float box_radisu = sqrt(a_squared + octreeBox.width*octreeBox.width);
//         float distanceToCenter = octreeBox.center.distanceToLine(rayOrigin,rayDir);

//         if(distanceToCenter <= box_radisu) return true;
//         return false;


//     }

    static BoundingBox computeBoundingBoxFromPoints(const QVector<QVector3D>& points)
    {
        BoundingBox res;


        for(unsigned int i=0; i<points.size(); ++i)
        {

        }

        return res;
    }

    static void DrawAxis(QOpenGLShaderProgram*& shader, QVector3D center, float radius)
    {

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

        shader->setUniformValue("u_color",QVector3D(0.5,0.5,0.0));
        glBegin(GL_LINES);
            glVertex3f(center.x(),center.y(), center.z());
            glVertex3f(center.x()+radius, center.y(), center.z());
        glEnd();
        shader->setUniformValue("u_color",QVector3D(0.5,0.0,0.5));
        glBegin(GL_LINES);
            glVertex3f(center.x(),center.y(), center.z());
            glVertex3f(center.x(), center.y()+radius, center.z());
        glEnd();
        shader->setUniformValue("u_color",QVector3D(0.0,0.5,0.5));
        glBegin(GL_LINES);
            glVertex3f(center.x(),center.y(), center.z());
            glVertex3f(center.x(), center.y(), center.z()+radius);
        glEnd();
    }
};

#endif // UTILS_H
