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

    // original 2D rugosity (before scaling)
    QVector<QVector<QVector2D>> m_2D_curves;
    QVector<QVector2D> m_2D_heights;

    float original_y_range, original_x_range, original_x_min, original_y_min;  // 用于scale的一些数值
    float scaled_x_max, scaled_y_max;  // scale后的数值
    float ui_scale_value = 1.0f;

    // visulized 2D rugosity (after scaling)
    QVector<QVector<QVector2D>> m_2D_curves_scaled;
    QVector<QVector2D> m_2D_heights_scaled;
    enum DISPLAY_MODE
    {
        _Normal,
        _OnlyLine,
    } m_displayMode;

    enum PROJECT_MODE{_Perspective, _Ortho} m_projectMode;

    QTimer m_viewingTimer;

    void SetEyeDist(const QVector3D& c) { this->m_eyeDist = c;}

    bool m_isWireFrame = true;
    bool m_isShowBVH = true;
    bool m_isBarkTextured;
    bool m_isShowSeg;


    // 参数maxY和minY是纵坐标轴的范围
    QVector<QVector<QVector2D>> Get2DVersionCurves(float &minY, float &maxY);  // Get Rugosity (cross-section, multi-curves)
    QVector<QVector2D> Get2DHeightMap(const QVector<QVector<QVector2D>>& curves);  // Get Rugosity (Heights)

    void Compute_XY_Range(QVector<QVector<QVector2D>>& curves, QVector<QVector2D>& heights);
    void Scale_2D_Rugosity(float scale=1.0f);

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

    bool isPressRightButton = false;
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

signals:
    void sig_updateChart(float minY, float maxY);

private slots:
    void OnViewingTimer();
};

#endif // GLWIDGET_H
