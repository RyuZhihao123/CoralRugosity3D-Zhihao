#include "object.h"
#define VERTEX_ATTRIBUTE  0
#define COLOUR_ATTRIBUTE  1
#define NORMAL_ATTRIBUTE  2
#define TEXTURE_ATTRIBUTE 3
#include <cstring>

bool isUsePLYFormatTexcoords = false;

Object::Object()
{

}

void Object::SetTextures(const QString &filepath)
{
    if(m_texture!=nullptr)
    {
        m_texture->release();
        m_texture->destroy();
        if(m_texture!=nullptr) delete m_texture;
    }

    QImage img(filepath);
    m_texture = new QOpenGLTexture(img);

    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
}

void Object::InverseY_Axis()
{
    for(int i=0; i<m_vertices.size(); ++i)
        m_vertices[i].setY(-m_vertices[i].y());
}

void Object::FlipTexcoords()
{
    for(int i=0; i<m_texcoords.size(); ++i)
    {
        m_texcoords[i].setY(1.0-m_texcoords[i].y());
    }
}

void Object::ResetAll()
{
    if(this->m_meshVBO.vbo.isCreated()) this->m_meshVBO.vbo.release();

    this->m_vertices.clear();
    this->m_normals.clear();
    this->m_colors.clear();
    this->m_texcoords.clear();
    this->m_faceids.clear();
    this->m_dictPtToFace.clear();

    this->octreeList.clear();

    if(m_texture!=nullptr)
    {
        delete m_texture;
    }
    m_texture = nullptr;

    isUsePLYFormatTexcoords = false;
}


void Object::DrawMeshVBO(QOpenGLShaderProgram *&program, const QMatrix4x4 &modelMat)
{

    if(this->m_meshVBO.count <=0 ||!this->m_meshVBO.vbo.isCreated())
        return;

    program->setUniformValue("mat_model",modelMat);

    m_meshVBO.vbo.bind();

    program->enableAttributeArray(VERTEX_ATTRIBUTE);
    program->enableAttributeArray(NORMAL_ATTRIBUTE);
    if(this->hasTexture()) program->enableAttributeArray(TEXTURE_ATTRIBUTE);
    else if(this->hasColor()) program->enableAttributeArray(COLOUR_ATTRIBUTE);

    if(this->hasTexture() == false && this->hasColor() == false)
    {
        program->setAttributeBuffer(VERTEX_ATTRIBUTE,   GL_FLOAT, 0,                3,6*sizeof(GLfloat));
        program->setAttributeBuffer(NORMAL_ATTRIBUTE,   GL_FLOAT, 3*sizeof(GLfloat),3,6*sizeof(GLfloat));
    }
    else
    {
        program->setAttributeBuffer(VERTEX_ATTRIBUTE,   GL_FLOAT, 0,                3,9*sizeof(GLfloat));
        program->setAttributeBuffer(NORMAL_ATTRIBUTE,   GL_FLOAT, 3*sizeof(GLfloat),3,9*sizeof(GLfloat));
        if(this->hasTexture())
            program->setAttributeBuffer(TEXTURE_ATTRIBUTE,  GL_FLOAT, 6*sizeof(GLfloat),3,9*sizeof(GLfloat));
        else if(this->hasColor())
            program->setAttributeBuffer(COLOUR_ATTRIBUTE,   GL_FLOAT, 6*sizeof(GLfloat),3,9*sizeof(GLfloat));

    }

    glDrawArrays(GL_TRIANGLES,0,this->m_meshVBO.count);

}

void Object::CreateMeshVBO(const std::string& filepath)
{
    this->ResetAll();

    // 读取二进制数据
    std::unique_ptr<std::istream> file_stream;
    file_stream.reset(new std::ifstream(filepath, std::ios::binary));

    // 文件不存在
    if (!file_stream || file_stream->fail())
        return;

    file_stream->seekg(0, std::ios::end);
    const float size_mb = file_stream->tellg() * float(1e-6);
    file_stream->seekg(0, std::ios::beg);

    // 创建PlyFile对象
    tinyply::PlyFile file;
    file.parse_header(*file_stream);

    std::shared_ptr<tinyply::PlyData> vertices_byte, normals_byte, colors_byte, texcoords_byte, faces_byte, tripstrip, plyTexcoords_byte;
    try { vertices_byte = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
    catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }
    try { normals_byte = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
    catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }
    try { colors_byte = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }); }
    catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }
    if (colors_byte == nullptr)
    {
        try { colors_byte = file.request_properties_from_element("vertex", { "r", "g", "b", "a" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }
    }
    try { texcoords_byte = file.request_properties_from_element("vertex", { "u", "v" }); }
    catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }
    try { faces_byte = file.request_properties_from_element("face", { "vertex_indices" }, 3); }
    catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }
    try { plyTexcoords_byte = file.request_properties_from_element("face", { "texcoord" }, 6); }
    catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }


    file.read(*file_stream);

    if (vertices_byte)   std::cout << "\tRead " << vertices_byte->count  << " total vertices "<< std::endl;
    if (normals_byte)    std::cout << "\tRead " << normals_byte->count   << " total vertex normals " << std::endl;
    if (colors_byte)     std::cout << "\tRead " << colors_byte->count << " total vertex colors " << std::endl;
    if (texcoords_byte)  std::cout << "\tRead " << texcoords_byte->count << " total vertex texcoords " << std::endl;
    if (faces_byte)      std::cout << "\tRead " << faces_byte->count     << " total faces (triangles) " << std::endl;
    if (tripstrip)  std::cout << "\tRead " << (tripstrip->buffer.size_bytes() / tinyply::PropertyTable[tripstrip->t].stride) << " total indicies (tristrip) " << std::endl;
    if (plyTexcoords_byte) std::cout << "\tRead " << (plyTexcoords_byte->buffer.size_bytes() / tinyply::PropertyTable[plyTexcoords_byte->t].stride) << " total PLY texcoords" << std::endl;

    std::vector<float3> _pos, _norms,_colors;
    std::vector<float2> _texcoords;
    std::vector<float6> _plyTexcoords;
    std::vector<uint3> _tris;
    if (vertices_byte) _pos.resize(vertices_byte->count);
    if (normals_byte) _norms.resize(normals_byte->count);
    if (colors_byte) _colors.resize(colors_byte->count);
    if (faces_byte) _tris.resize(faces_byte->count);
    if (texcoords_byte) {
        _texcoords.resize(texcoords_byte->count);
        isUsePLYFormatTexcoords = false;
    }
    if(_texcoords.size() == 0 && plyTexcoords_byte){
         _plyTexcoords.resize(plyTexcoords_byte->count);
         isUsePLYFormatTexcoords = true;
    }

    // _pos
    std::memcpy(_pos.data(), vertices_byte->buffer.get(), (size_t)( vertices_byte->buffer.size_bytes()));

    if(normals_byte!=nullptr)
    {
        std::memcpy(_norms.data(), normals_byte->buffer.get(), (size_t)( normals_byte->buffer.size_bytes()));
    }
    if(colors_byte!=nullptr)
    {
        std::memcpy(_colors.data(), colors_byte->buffer.get(), (size_t)( colors_byte->buffer.size_bytes()));
    }
    if(texcoords_byte!=nullptr)
    {
        std::memcpy(_texcoords.data(), texcoords_byte->buffer.get(), (size_t)( texcoords_byte->buffer.size_bytes()));
    }
    if(plyTexcoords_byte!=nullptr)
    {
        std::memcpy(_plyTexcoords.data(), plyTexcoords_byte->buffer.get(), (size_t)( plyTexcoords_byte->buffer.size_bytes()));
    }
    if(faces_byte!=nullptr)
    {
        std::memcpy(_tris.data(), faces_byte->buffer.get(), (size_t)(  faces_byte->buffer.size_bytes()));
    }

    // 读取到数组中去
    for(int i=0; i<_pos.size(); ++i)
        m_vertices.push_back(QVector3D(_pos[i].x,_pos[i].y,_pos[i].z));
    for(int i=0; i<_colors.size(); ++i)
        m_colors.push_back(QVector3D(_colors[i].x,_colors[i].y,_colors[i].z));
    for(int i=0; i<_texcoords.size(); ++i)
        m_texcoords.push_back(QVector2D(_texcoords[i].x,_texcoords[i].y));
    for(int i=0; i<_tris.size(); ++i)
        m_faceids.push_back(_tris[i]);

    if(m_texcoords.size() == 0)
    {
        for(int i=0; i<_plyTexcoords.size(); ++i)
        {

            //qDebug()<<_plyTexcoords[i].u1<<_plyTexcoords[i].v1 << _plyTexcoords[i].u2<<_plyTexcoords[i].v2<<_plyTexcoords[i].u3<<_plyTexcoords[i].v3;
            m_texcoords.push_back(QVector2D(_plyTexcoords[i].u1,_plyTexcoords[i].v1));
            m_texcoords.push_back(QVector2D(_plyTexcoords[i].u2,_plyTexcoords[i].v2));
            m_texcoords.push_back(QVector2D(_plyTexcoords[i].u3,_plyTexcoords[i].v3));
        }
    }

    if(_norms.size() > 0)
    {
        for(int i=0; i<_norms.size(); ++i)
            m_normals.push_back(QVector3D(_norms[i].x,_norms[i].y,_norms[i].z));
    }
    else
    {
        this->m_normals.fill(QVector3D(0,0,0), m_vertices.size());
        for(int i=0; i<this->m_faceids.size(); ++i)
        {
            int id1 = m_faceids[i].x;
            int id2 = m_faceids[i].y;
            int id3 = m_faceids[i].z;

            QVector3D v1 = m_vertices[id1];  // v1
            QVector3D v2 = m_vertices[id2];  // v2
            QVector3D v3 = m_vertices[id3];  // v3

            QVector3D e1 = v2 - v1;
            QVector3D e2 = v2 - v3;

            QVector3D normal = QVector3D::crossProduct(e1, e2);
            if(normal.y()<0)
                normal = -normal;

            m_normals[id1] += normal;
            m_normals[id2] += normal;
            m_normals[id3] += normal;
        }
        for(int i=0; i<m_normals.size(); ++i)
            m_normals[i].normalize();
    }


    // 移动到原点
    QVector3D center(0,0,0);
    for(int i=0; i<m_vertices.size(); ++i)
    {
        center += m_vertices[i];
    }
    center/=m_vertices.size();
    for(int i=0; i<m_vertices.size(); ++i)
    {
        m_vertices[i] -= center;
    }

    // 反转Y轴：
    this->InverseY_Axis();

    // 镜像翻转纹理图片
    this->FlipTexcoords();

    QString defaultTexturePath = GlobalTools::checkDefaultTextureExists(QString::fromStdString(filepath));
    qDebug()<<"查找默认纹理文件:" << defaultTexturePath;
    if(defaultTexturePath != "")
        this->SetTextures(defaultTexturePath);

    // 生成VBO
    this->UpdateMeshs();
}

void Object::UpdateMeshs()
{
    QVector<GLfloat> data;
    for(int i=0; i<this->m_faceids.size(); ++i)
    {
        if(i%(int)(m_faceids.size()/10.0f) == 0)
            qDebug()<<"加载进度"<<(int)(i/(m_faceids.size()/10.0f))*10 <<"%";

        int id1 = m_faceids[i].x;
        int id2 = m_faceids[i].y;
        int id3 = m_faceids[i].z;

        QVector3D v1 = m_vertices[id1];  // v1
        QVector3D v2 = m_vertices[id2];  // v2
        QVector3D v3 = m_vertices[id3];  // v3

        QVector3D n1,n2,n3;
        n1 = m_normals[id1];  // n1
        n2 = m_normals[id2];  // n2
        n3 = m_normals[id3];  // n3


        QVector3D c1,c2,c3;
        if(this->hasColor())
        {
            c1 = m_colors[id1];
            c2 = m_colors[id2];
            c3 = m_colors[id3];
        }

        QVector2D t1,t2,t3;
        if(this->hasTexture() && isUsePLYFormatTexcoords == false)
        {
            t1 = m_texcoords[id1];
            t2 = m_texcoords[id2];
            t3 = m_texcoords[id3];
        }
        else if(this->hasTexture() && isUsePLYFormatTexcoords == true)
        {

            t1 = m_texcoords[i*3+0];
            t2 = m_texcoords[i*3+1];
            t3 = m_texcoords[i*3+2];
                       // qDebug()<<t1<<t2<<t3;
        }

        data.push_back(v1.x()); data.push_back(v1.y()); data.push_back(v1.z());
        data.push_back(n1.x()); data.push_back(n1.y()); data.push_back(n1.z());
        if(this->hasTexture()) {
            data.push_back(t1.x()); data.push_back(t1.y()); data.push_back(1.0f);
        }
        else if(this->hasColor()){
            data.push_back(c1.x()); data.push_back(c1.y()); data.push_back(c1.z());
        }

        data.push_back(v2.x()); data.push_back(v2.y()); data.push_back(v2.z());
        data.push_back(n2.x()); data.push_back(n2.y()); data.push_back(n2.z());
        if(this->hasTexture()) {
            data.push_back(t2.x()); data.push_back(t2.y()); data.push_back(1.0f);
        }
        else if(this->hasColor()){
            data.push_back(c2.x()); data.push_back(c2.y()); data.push_back(c2.z());
        }

        data.push_back(v3.x()); data.push_back(v3.y()); data.push_back(v3.z());
        data.push_back(n3.x()); data.push_back(n3.y()); data.push_back(n3.z());
        if(this->hasTexture()) {
            data.push_back(t3.x()); data.push_back(t3.y()); data.push_back(1.0f);
        }
        else if(this->hasColor()){
            data.push_back(c3.x()); data.push_back(c3.y()); data.push_back(c3.z());
        }
    }


    m_meshVBO.count = data.size()/6;
    if(this->hasTexture())    m_meshVBO.count = data.size()/9;
    else if(this->hasColor()) m_meshVBO.count = data.size()/9;

    qDebug()<<"创建"<<m_meshVBO.vbo.create();
    m_meshVBO.vbo.bind();
    m_meshVBO.vbo.allocate(data.constData(),data.count()*sizeof(GLfloat));



    qDebug()<<"VBO mesh count:" << m_meshVBO.count << m_meshVBO.vbo.isCreated();
}

void Object::CreateLineVBO(const Curve &curve)
{
    QVector<GLfloat> data;

    for(int i=0; i<curve.pts.size(); ++i)
    {
        QVector3D pt = curve.pts[i];
        data.append(pt.x());data.append(pt.y());data.append(pt.z());
    }

    m_meshVBO.count = data.size()/3;

    qDebug()<<"创建LineVBO"<<m_meshVBO.vbo.create() << m_meshVBO.count;
    m_meshVBO.vbo.bind();
    m_meshVBO.vbo.allocate(data.constData(),data.count()*sizeof(GLfloat));
}

void Object::DrawLineVBO(QOpenGLShaderProgram *&program, const QMatrix4x4 &modelMat)
{

    if(this->m_meshVBO.count <=0 ||!this->m_meshVBO.vbo.isCreated())
        return;

    program->setUniformValue("mat_model",modelMat);

    m_meshVBO.vbo.bind();

    program->enableAttributeArray(VERTEX_ATTRIBUTE);
    program->setAttributeBuffer(VERTEX_ATTRIBUTE,   GL_FLOAT, 0, 3, 3*sizeof(GLfloat));


    glDrawArrays(GL_LINE_STRIP,0, this->m_meshVBO.count);
}


