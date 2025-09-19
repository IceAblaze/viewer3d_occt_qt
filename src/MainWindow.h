#pragma once
#include <QMainWindow>
#include <QString>
class OcctViewer; class QPushButton;
class MainWindow : public QMainWindow {
    Q_OBJECT
public: explicit MainWindow(QWidget* parent=nullptr); ~MainWindow() override;
private slots: void onLoadJson(); void onResetView(); void onToggleTools();
private:
    OcctViewer* m_viewer=nullptr; QPushButton *m_btnLoad=nullptr,*m_btnReset=nullptr,*m_btnToggleTools=nullptr;
    bool m_toolsVisible=false; QString m_currentJsonPath;
    void buildAndDisplayModel(const QString& jsonPath);
};
