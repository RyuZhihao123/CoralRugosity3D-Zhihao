#ifndef GLWIDGET_H
#define GLWIDGET_H
#include "object.h"
#include "rugosityalgorithm.h"

#define PI 3.1415926

class GLWidget  : public QOpenGLWidget , public QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();

    RugosityAlgorithm* m_rugosity;


    enum DISPLAY_MODE
    {
        _Normal,
        _OnlyLine,
    } m_displayMode;

    enum PROJECT_MODE{_Perspective, _Ortho} m_projectMode;

    QTimer m_viewingTimer;

    void SetEyeDist(const QVector3D& c) { this->m_eyeDist = c;}

    bool m_isShowLeaves;
    bool m_polygonMode;
    bool m_isBarkTextured;
    bool m_isShowSeg;

protected:
    void initializeGL();
    void resizeGL(int w, int h);

    void paintGL();

    // shader programs
    QOpenGLShaderProgram* m_pointProgram;
    QOpenGLShaderProgram* m_skeletonProgram;
    QOpenGLShaderProgram* m_meshProgram;

    // Textures
    QOpenGLTexture* m_leafTexture;

    void initShaders(QOpenGLShaderProgram *&m_program, const QString &shaderName, bool v, bool c, bool n, bool t);
    void setupShaders(QOpenGLShaderProgram*& m_program);
    void setupTexture(QOpenGLTexture*& texture,const QString& filename);

    // rendering related
    QMatrix4x4 m_projectMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_modelMat;
    QVector3D m_eyePos,m_eyeDist;

    QPoint m_clickpos;
    bool isPressLeftButton = false;

    bool isPressRightButton = true;
    QVector2D m_startPt, m_endPt;

    double m_horAngle,m_verAngle;

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    double distance;
    double scale;
    void wheelEvent(QWheelEvent* event);

    void keyPressEvent(QKeyEvent* e);

private:

    Ray GetRayFromScreenPos(QVector2D screenPoint);

private slots:
    void OnViewingTimer();
};

#endif // GLWIDGET_H
