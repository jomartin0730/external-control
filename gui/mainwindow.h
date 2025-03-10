#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QProgressBar>
#include "../include/client.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    QTimer timer_;
    QTimer update_timer_;
    QProgressBar *progress_bar;

    Ui::MainWindow* ui;
    int progress = 0;
    void keyPressEvent(QKeyEvent* event) override;
public:
    rb::Client* client_{nullptr};
    systemSTAT* systemStat_ = &client_->systemStat_;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void CmdConnected();
    void DataConnected();
    void RequestData();
    void UpdateSystem();
    void setBaseSpeed(float value);
    void OnMotionPause();
    void OnMotionResume();
    void OnMotionStop();
    void OnCollisionResume();
    void print(QString value);
};


