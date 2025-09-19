#pragma once
#include <QOpenGLWidget>
#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <AIS_Shape.hxx>
#include <vector>

class OcctViewer : public QOpenGLWidget {
    Q_OBJECT
public:
    explicit OcctViewer(QWidget* parent=nullptr);
    ~OcctViewer() override;
    void displayShape(const Handle(AIS_Shape)& shape);
    void setToolShapes(const std::vector<Handle(AIS_Shape)>& tools);
    void showToolCylinders(bool show);
    void resetView();
protected:
    void initializeGL() override; void resizeGL(int w,int h) override; void paintGL() override;
    void mousePressEvent(QMouseEvent*) override; void mouseMoveEvent(QMouseEvent*) override; void wheelEvent(QWheelEvent*) override;
private:
    Handle(V3d_Viewer) m_viewer; Handle(V3d_View) m_view; Handle(AIS_InteractiveContext) m_ctx;
    Handle(AIS_Shape) m_mainShape; std::vector<Handle(AIS_Shape)> m_toolShapes; QPoint m_lastPos;
};
