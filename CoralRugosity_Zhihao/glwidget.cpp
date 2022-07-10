#include "glwidget.h"

#define VERTEX_ATTRIBUTE  0
#define COLOUR_ATTRIBUTE  1
#define NORMAL_ATTRIBUTE  2
#define TEXTURE_ATTRIBUTE 3


GLWidget::GLWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);

    m_rugosity = new RugosityAlgorithm();

    m_leafTexture = NULL;
    m_meshProgram = NULL;
    m_skeletonProgram = NULL;
    m_pointProgram = NULL;

    m_polygonMode = true;
    m_isBarkTextured = false;
    m_isShowSeg = false;

    m_projectMode = _Perspective;
    this->m_displayMode = DISPLAY_MODE::_Normal;
    m_horAngle = 45.0f;
    m_verAngle = 0.0f;
    distance = 10.0f;
    scale = 1.0f;
    m_eyeDist = QVector3D(0.0f,0.0f,0.0f);


    connect(&m_viewingTimer,SIGNAL(timeout()),this,SLOT(OnViewingTimer()));
}


void GLWidget::initShaders(QOpenGLShaderProgram*& m_program,const QString& shaderName,
                           bool v = false,bool c = false, bool n = false, bool t=false)
{
    m_program = new QOpenGLShaderProgram(this);

    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,  QString(":/shader/res/%1VShader.glsl").arg(shaderName));
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,QString(":/shader/res/%1FShader.glsl").arg(shaderName));

    if(v)
        m_program->bindAttributeLocation("a_position",VERTEX_ATTRIBUTE);
    if(c)
        m_program->bindAttributeLocation("a_color"   ,COLOUR_ATTRIBUTE);
    if(n)
        m_program->bindAttributeLocation("a_normal"  ,NORMAL_ATTRIBUTE);
    if(t)
        m_program->bindAttributeLocation("a_texCoord",TEXTURE_ATTRIBUTE);

    m_program->link();
    m_program->bind();
}


void GLWidget::setupShaders(QOpenGLShaderProgram *&m_program)
{
    m_program->bind();

    m_program->setUniformValue("mat_projection",m_projectMatrix);
    m_program->setUniformValue("mat_view",m_viewMatrix);
}

void GLWidget::setupTexture(QOpenGLTexture *&texture, const QString &filename)
{
    if(texture)
        delete texture;

    QImage img(filename);
    texture = new QOpenGLTexture(img);

    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
}


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(1.0,1.0,1.0,1.0);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_TEXTURE_2D);


    initShaders(m_pointProgram,"Points",true,true,false,false);
    initShaders(m_skeletonProgram,"Graph",true,false,false,false);
    initShaders(m_meshProgram,"bark",true,false,true,true);

    //setupTexture(m_leafTexture,":/img/res/leaf/叶子.png");
    qsrand(QTime::currentTime().msec()*QTime::currentTime().second());

    // 加载默认的coral mesh
    m_rugosity->GetInputMesh().CreateMeshVBO("D:/Projects/HKUST_VGD/Coral/videos/video1/coral model (small image size)/scene_dense_mesh_refine_texture.ply");
    m_rugosity->BuildKDTree(); // 创建Kd树

    // 设定最原始配置的curve
    QVector<QVector3D> tmp;

    tmp.append(QVector3D(0.0,0.0,0.0));
    tmp.append(QVector3D(0.0,0.0,1.0));
    tmp.append(QVector3D(0.0,1.0,2.0));
    tmp.append(QVector3D(2.0,-1.0,-2.0));
    tmp.append(QVector3D(5.0,10.0,0.0));

    Curve tmpCurve;
    tmpCurve.pts = tmp;
    m_rugosity->m_curves.append(tmpCurve);

    m_rugosity->UpdateCurrentMesh();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,width(),height());

    m_projectMatrix.setToIdentity();

    if(m_projectMode == _Perspective)
        m_projectMatrix.perspective(45.0f,(float)w/(float)h,0.1f,100000.0f);
    else if(m_projectMode == _Ortho)
        m_projectMatrix.ortho(-400,400,-400,400,-100.0,1000);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(m_projectMode == _Perspective)
    {
        m_eyePos = m_eyeDist+QVector3D(scale*distance*cos(PI*m_verAngle/180.0)*cos(PI*m_horAngle/180.0),
                                       scale*distance*sin(PI*m_verAngle/180.0),
                                       scale*distance*cos(PI*m_verAngle/180.0)*sin(PI*m_horAngle/180.0));
        m_viewMatrix.setToIdentity();
        m_viewMatrix.lookAt(m_eyePos,m_eyeDist,QVector3D(0,1,0));
        m_modelMat.setToIdentity();
    }
    else if(m_projectMode == _Ortho)
    {
        m_viewMatrix.setToIdentity();
        m_modelMat.setToIdentity();
    }


    //    if(!m_polygonMode)
    //        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    //    else
//    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

     glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    {
        glLineWidth(1);
        // 绘制mesh
        setupShaders(m_meshProgram);
        glEnable(GL_TEXTURE0);

        m_meshProgram->setUniformValue("texture",0);
        if(m_rugosity->GetInputMesh().hasTexture())
        {
            m_rugosity->GetInputMesh().m_texture->bind();
            m_meshProgram->setUniformValue("isTextured",1);
        }
        else
            m_meshProgram->setUniformValue("isTextured",0);

        m_meshProgram->setUniformValue("viewPos",m_eyePos);
        m_meshProgram->setUniformValue("isLighting",false);
        m_meshProgram->setUniformValue("isBark",false);
        m_meshProgram->setUniformValue("isShowSeg",false);
        //m_tree->DrawTreeMeshVBO(m_meshProgram,m_modelMat);
        m_rugosity->GetInputMesh().DrawMeshVBO(m_meshProgram,m_modelMat);

    }
    {
        // 绘制找到的curve
        glLineWidth(4);
        setupShaders(this->m_skeletonProgram);
        m_skeletonProgram->setUniformValue("u_color",QVector3D(1,0,0));
        m_rugosity->RenderCurrentMesh(m_skeletonProgram,m_modelMat);
    }

    {
        //------------ 渲染其他 --------------------- //
        if(this->isPressRightButton)
        {
            // 绘制一个选取矩形
            glColor3f(0.0,0.0,1.0);

            m_skeletonProgram->bind();

            m_skeletonProgram->setUniformValue("mat_projection",QMatrix4x4());
            m_skeletonProgram->setUniformValue("mat_view",QMatrix4x4());

            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

            glLineWidth(2.0f);

            float x1 = 2*m_startPt.x()/(float)width()-1.0;
            float y1 = -2*m_startPt.y()/(float)height()+1.0;
            float x2 = 2*m_endPt.x()/(float)width()-1.0;
            float y2 = -2*m_endPt.y()/(float)height()+1.0;

            glBegin(GL_LINES);
                glVertex2f(x1, y1);
                glVertex2f(x2, y2);
            glEnd();


            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

        }
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton)
    {
        isPressRightButton = true;
        m_startPt = QVector2D(event->pos());
        return;
    }
    m_clickpos = event->pos();


}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton)
    {
        isPressRightButton = true;

        m_endPt = QVector2D(event->pos());
        update();

        return;
    }
    if(event->buttons() == Qt::LeftButton)
    {
        QPoint cur = event->pos();

        double dx = (cur.x() - m_clickpos.x())/5.0;
        double dy = (cur.y() - m_clickpos.y())/5.0;

        m_horAngle +=dx;
        if(m_verAngle+dy <90.0 && m_verAngle+dy>-90.0)
            m_verAngle +=dy;

        m_clickpos = cur;
        update();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(isPressRightButton)
    {
        isPressRightButton = false;
        update();
        return;
    }
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_W)
        m_eyeDist.setY(m_eyeDist.y()+5);
    if(e->key() == Qt::Key_S)
        m_eyeDist.setY(m_eyeDist.y()-5);
    if(e->key() == Qt::Key_C)
        m_polygonMode = ! m_polygonMode;
    this->update();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    double ds = 0.03;
    if(event->delta()>0 && scale-ds>0)
        scale-=ds;
    else if(event->delta()<0)
        scale+=ds;
    update();
}

void GLWidget::OnViewingTimer()
{
    m_horAngle +=1;
    update();
}

GLWidget::~GLWidget(){}
