#include "OcctViewer.h"
#include <QMouseEvent><QOpenGLContext>
#include <Aspect_DisplayConnection.hxx><OpenGl_GraphicDriver.hxx><Graphic3d_GraphicDriver.hxx>
#include <WNT_Window.hxx><Xw_Window.hxx><Prs3d_Drawer.hxx><Prs3d_LineAspect.hxx>
#include <Quantity_Color.hxx><V3d_Viewer.hxx><V3d_View.hxx>

OcctViewer::OcctViewer(QWidget* parent):QOpenGLWidget(parent){
    setAttribute(Qt::WA_PaintOnScreen); setAttribute(Qt::WA_NoSystemBackground); setAutoFillBackground(false);
}
OcctViewer::~OcctViewer(){ m_ctx.Nullify(); m_view.Nullify(); m_viewer.Nullify(); }
void OcctViewer::initializeGL(){
    Handle(Aspect_DisplayConnection) disp=new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver) driver=new OpenGl_GraphicDriver(disp);
    m_viewer=new V3d_Viewer(driver); m_viewer->SetDefaultLights(); m_viewer->SetLightOn();
    m_viewer->SetDefaultBackgroundColor(Quantity_NOC_WHITE);
    m_view=m_viewer->CreateView();
    WId wid=(WId)winId();
#if defined(_WIN32)
    Handle(WNT_Window) win=new WNT_Window((Aspect_Handle)wid);
    m_view->SetWindow(win); if(!win->IsMapped()) win->Map();
#elif defined(__linux__)
    Handle(Xw_Window) win=new Xw_Window(disp,(Window)wid);
    m_view->SetWindow(win); if(!win->IsMapped()) win->Map();
#endif
    m_ctx=new AIS_InteractiveContext(m_viewer); m_ctx->SetDisplayMode(AIS_Shaded,Standard_True);
    Handle(Prs3d_Drawer) dwr=m_ctx->DefaultDrawer(); dwr->SetFaceBoundaryDraw(Standard_True);
    dwr->SetFaceBoundaryAspect(new Prs3d_LineAspect(Quantity_NOC_BLACK,Aspect_TOL_SOLID,1.0));
    m_view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER,Quantity_NOC_GRAY,0.08,V3d_ZBUFFER);
    m_view->SetProj(V3d_XnegYnegZneg);
}
void OcctViewer::resizeGL(int,int){ if(!m_view.IsNull()) m_view->MustBeResized(); }
void OcctViewer::paintGL(){ if(!m_view.IsNull()) m_view->Redraw(); }
void OcctViewer::displayShape(const Handle(AIS_Shape)& s){
    if(s.IsNull()||m_ctx.IsNull()) return; if(!m_mainShape.IsNull()) m_ctx->Remove(m_mainShape,Standard_False);
    m_mainShape=s; m_ctx->Display(m_mainShape,Standard_True); if(!m_view.IsNull()) m_view->FitAll();
}
void OcctViewer::setToolShapes(const std::vector<Handle(AIS_Shape)>& t){ m_toolShapes=t; for(auto& a:m_toolShapes){ if(!a.IsNull()) a->SetDisplayMode(AIS_Shaded);} }
void OcctViewer::showToolCylinders(bool show){
    if(m_ctx.IsNull()) return; for(auto& a:m_toolShapes){ if(a.IsNull()) continue;
        if(show){ if(!m_ctx->IsDisplayed(a)) m_ctx->Display(a,Standard_False); a->SetTransparency(0.5f); }
        else{ if(m_ctx->IsDisplayed(a)) m_ctx->Erase(a,Standard_False); } }
    if(!m_view.IsNull()) m_view->Redraw();
}
void OcctViewer::resetView(){ if(!m_view.IsNull()){ m_view->FitAll(); m_view->ZFitAll(); } }
void OcctViewer::mousePressEvent(QMouseEvent* e){ m_lastPos=e->pos(); e->accept(); }
void OcctViewer::mouseMoveEvent(QMouseEvent* e){
    if(m_view.IsNull()) return; QPoint d=e->pos()-m_lastPos;
    if(e->buttons()&Qt::MiddleButton) m_view->Rotation(d.x(),d.y());
    else if(e->buttons()&Qt::LeftButton) m_view->Pan(d.x(),-d.y());
    m_lastPos=e->pos(); e->accept();
}
void OcctViewer::wheelEvent(QWheelEvent* e){
    if(!m_view.IsNull()){ int delta=e->angleDelta().y(); if(delta!=0){ Standard_Real f= delta>0?1.1:0.9; m_view->SetZoom(m_view->Camera()->Scale()*f);} }
    e->accept();
}
