#ifndef RAINBOWCLIENT_H
#define RAINBOWCLIENT_H

#include <iostream>
#include <cstring>
#include <chrono>
#include <fstream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <memory>
#include <QtNetwork>
#include <QTimer>
#include <QString>
#include <QByteArray>
#include "common.h"

namespace rb {

class Client : public QObject {
private:
    QTimer timer_;
    QByteArray result_data_;
public:
    Client();
    ~Client();
    RobotValue val_;
    systemSTAT systemStat_;
    // systemCONFIG systemConfig_;
    // systemPOPUP  systemPopup_;
    QTcpSocket* cmd_socket_{nullptr};
    QTcpSocket* data_socket_{nullptr};

    void CobotInit();
    void ProgramMode_Real();
    bool BaseSpeedChange(double speed);

    void MoveJ(double* joint, int speed, int acceleration);
    void MoveL(double* pose, int speed, int acceleration);

    void SetDigitalOut(int port, int signal);
    void SetDigitalOutAll(int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7, int d8, int d9, int d10, int d11, int d12, int d13, int d14, int d15);
    void SetToolOutAll(int volt, int d0, int d1);

    void MotionPause();
    void MotionResume();
    void MotionStop();
    void CollisionResume();

    // std::unordered_map<std::string, std::tuple<std::vector<double>, int>> pose_data_;
    // void MakePose();
    // double* Position(std::string cobot_pose);
    // double GetDistance(const double* pose);
    // bool IsHomePosition();

public slots:
    void TryConnect(QTcpSocket* socket, const QString& ip, int port);
    void CommandRead();
    void DataRead();

};
}
#endif // RAINBOWCLIENT_H
