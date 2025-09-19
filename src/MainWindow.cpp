#include "MainWindow.h"
#include "OcctViewer.h"
#include <QPushButton><QWidget><QHBoxLayout><QVBoxLayout><QFileDialog><QMessageBox><QFile>
#include <QJsonDocument><QJsonArray><QJsonObject>
#include <TopoDS_Solid.hxx><BRepPrimAPI_MakeBox.hxx><BRepPrimAPI_MakeCylinder.hxx><BRepAlgoAPI_Cut.hxx>
#include <AIS_Shape.hxx><Quantity_Color.hxx><Standard_Failure.hxx>
#include <gp_Pnt.hxx><gp_Dir.hxx><gp_Vec.hxx><gp_Ax2.hxx>

MainWindow::MainWindow(QWidget* parent):QMainWindow(parent){
    setWindowTitle("OpenCascade JSON Viewer 3D"); auto *central=new QWidget(this); setCentralWidget(central);
    auto *mainL=new QVBoxLayout(central); central->setLayout(mainL);
    m_viewer=new OcctViewer(central); mainL->addWidget(m_viewer,1);
    auto *row=new QHBoxLayout(); m_btnLoad=new QPushButton("Carica JSON",this);
    m_btnReset=new QPushButton("Reset View",this); m_btnToggleTools=new QPushButton("Mostra Tools (cilindri)",this);
    row->addWidget(m_btnLoad); row->addWidget(m_btnReset); row->addWidget(m_btnToggleTools); mainL->addLayout(row);
    connect(m_btnLoad,&QPushButton::clicked,this,&MainWindow::onLoadJson);
    connect(m_btnReset,&QPushButton::clicked,this,&MainWindow::onResetView);
    connect(m_btnToggleTools,&QPushButton::clicked,this,&MainWindow::onToggleTools);
    const auto args=QCoreApplication::arguments(); if(args.size()>1){ m_currentJsonPath=args.at(1); buildAndDisplayModel(m_currentJsonPath); }
}
MainWindow::~MainWindow()=default;
void MainWindow::onLoadJson(){ const QString p=QFileDialog::getOpenFileName(this,tr("Seleziona file JSON"),{},tr("JSON Files (*.json)")); if(p.isEmpty())return; m_currentJsonPath=p; buildAndDisplayModel(m_currentJsonPath); }
void MainWindow::onResetView(){ if(m_viewer) m_viewer->resetView(); }
void MainWindow::onToggleTools(){ m_toolsVisible=!m_toolsVisible; if(m_viewer) m_viewer->showToolCylinders(m_toolsVisible);
    m_btnToggleTools->setText(m_toolsVisible?"Nascondi Tools (cilindri)":"Mostra Tools (cilindri)"); }
static bool isAxisAligned(const gp_Dir& d,double tol=1e-6){
    return (std::abs(std::abs(d.X())-1.0)<tol&&std::abs(d.Y())<tol&&std::abs(d.Z())<tol)||
           (std::abs(std::abs(d.Y())-1.0)<tol&&std::abs(d.X())<tol&&std::abs(d.Z())<tol)||
           (std::abs(std::abs(d.Z())-1.0)<tol&&std::abs(d.X())<tol&&std::abs(d.Y())<tol); }
void MainWindow::buildAndDisplayModel(const QString& path){
    QFile f(path); if(!f.open(QIODevice::ReadOnly)){ QMessageBox::critical(this,"Errore","Impossibile aprire il file JSON."); return; }
    const QJsonDocument doc=QJsonDocument::fromJson(f.readAll()); f.close();
    if(!doc.isArray()){ QMessageBox::critical(this,"Errore","Il JSON deve essere un array."); return; }
    const QJsonArray arr=doc.array(); if(arr.isEmpty()){ QMessageBox::critical(this,"Errore","JSON vuoto."); return; }
    const QJsonObject dims=arr.at(0).toObject();
    const double L=dims.value("length").toDouble(dims.value("x").toDouble(0));
    const double W=dims.value("width").toDouble(dims.value("y").toDouble(0));
    const double T=dims.value("thickness").toDouble(dims.value("z").toDouble(0));
    TopoDS_Solid block=BRepPrimAPI_MakeBox(L,W,T).Solid();
    std::vector<Handle(AIS_Shape)> tools; tools.reserve(std::max(0,arr.size()-1));
    const double EXTRA=15.0;
    for(int i=1;i<arr.size();++i){ const QJsonObject op=arr.at(i).toObject();
        if(!op.contains("depth")||!op.contains("diameter")||!op.contains("surfaceCenter")||!op.contains("processDirection")) continue;
        const double depth=op.value("depth").toDouble(); const double dia=op.value("diameter").toDouble();
        const QJsonObject c=op.value("surfaceCenter").toObject(); const QJsonObject d=op.value("processDirection").toObject();
        gp_Pnt entry(c.value("x").toDouble(),c.value("y").toDouble(),c.value("z").toDouble());
        gp_Vec dv(d.value("x").toDouble(),d.value("y").toDouble(),d.value("z").toDouble()); if(dv.Magnitude()<=0) continue; gp_Dir dir(dv);
        double depth_eff=depth; gp_Pnt entry_eff=entry; if(!isAxisAligned(dir)){ depth_eff+=EXTRA; entry_eff.Translate(gp_Vec(dir)*(-EXTRA)); }
        const double r=dia*0.5; gp_Ax2 ax(entry_eff,dir); TopoDS_Solid cyl=BRepPrimAPI_MakeCylinder(ax,r,depth_eff).Solid();
        Handle(AIS_Shape) t=new AIS_Shape(cyl); t->SetColor(Quantity_NOC_RED); t->SetTransparency(0.5f); tools.push_back(t);
        try{ block=TopoDS::Solid(BRepAlgoAPI_Cut(block,cyl).Shape()); }catch(Standard_Failure& e){ qWarning("Boolean cut fallita: %s", e.GetMessageString()); }
    }
    Handle(AIS_Shape) blockAIS=new AIS_Shape(block); blockAIS->SetColor(Quantity_NOC_BURLYWOOD);
    if(m_viewer){ m_viewer->displayShape(blockAIS); m_viewer->setToolShapes(tools); m_viewer->showToolCylinders(m_toolsVisible); }
}
