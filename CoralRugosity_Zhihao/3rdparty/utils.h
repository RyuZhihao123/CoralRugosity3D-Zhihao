#ifndef UTILS_H
#define UTILS_H
#include <QString>
#include <QFileInfo>

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


};

#endif // UTILS_H
