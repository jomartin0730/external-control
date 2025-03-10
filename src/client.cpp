#include "../include/client.h"

namespace rb {
    Client::Client() {
        memset(&systemStat_, 0, sizeof(systemStat_));
        // QTcpSocket 객체를 동적으로 할당
        cmd_socket_ = new QTcpSocket(this);
        data_socket_ = new QTcpSocket(this);
        std::cout << "create class" << std::endl;
    }
    Client::~Client() {
        std::cout << "delete class" << std::endl;
        delete cmd_socket_;
        delete data_socket_;
    }

    void Client::TryConnect(QTcpSocket* socket, const QString& ip, int port) {
        socket->connectToHost(QHostAddress(ip), port);
        std::cout << "Success to connect with Cobot" << std::endl;
    }

    void Client::CommandRead() {
        std::cout << "Cobot open CMD Close " << std::endl;
        QByteArray data = cmd_socket_->readAll();
        QString cmd_data = QString::fromUtf8(data);

        if ((cmd_data[0] == 'e') || (cmd_data[0] == 'i')) {
            std::vector<std::string> results;
            std::istringstream iss(cmd_data.toStdString());
            std::string token;
            while (std::getline(iss, token, '[') && std::getline(iss, token, ']')) {
                results.push_back(token);
            }
            if (results[0] == "code") {
                int error_id = stoi(results[1]);
            }
            else if (results[0] == "msg") {
                std::cout << "Error Message : " << results[1].c_str() << std::endl;
            }
            else if (results[0] == "motion_changed") {
                std::cout << "info : " << cmd_data.toStdString() << std::endl;
            }
            else {
                std::cout << "unknown : " << cmd_data.toStdString() << std::endl;
            }
        }

        if (cmd_data == "The command was executed\n") {
            std::cout << "CMD Success" << std::endl;
        }
        else {
            std::cout << "Unknown Message : " <<  cmd_data.toStdString() << std::endl;;
        }
        std::cout << "Cobot CMD Close " << std::endl;
    }

    void Client::DataRead() {
        QByteArray data = data_socket_->readAll();
        result_data_ += data;

        while (result_data_.length() > 4) {
            if (result_data_.at(0) == '$') {
                int size = (static_cast<unsigned char>(result_data_.at(2)) << 8) | static_cast<unsigned char>(result_data_.at(1));
                if (size <= result_data_.length()) {
                    switch (result_data_.at(3)) {
                        case 3: {
                            QThread::sleep(50);
                            std::memcpy(&systemStat_, result_data_.data(), sizeof(systemStat_));
                            result_data_.remove(0, sizeof(systemStat_));
                            break;
                        }
                        // case 4: {
                        //     std::memcpy(&systemConfig_, result_data_.data(), sizeof(systemConfig_));
                        //     result_data_.remove(0, sizeof(systemConfig_));
                        //     QString str = ui->textPrint->toPlainText();
                        //     break;
                        // }
                        // case 10: {
                        //     std::memcpy(&systemPopup_, result_data_.data(), sizeof(systemPopup_));
                        //     result_data_.remove(0, sizeof(systemPopup_));
                        //     break;
                        // }
                        default:
                            result_data_.remove(0, 1);
                            break;
                    }
                }
            }
            else {
                result_data_.remove(0, 1);
            }
        }
        std::cout << "Cobot Data Close " << std::endl;
    }

    void Client::CobotInit() {
        const QString message = "mc jall init";
        cmd_socket_->write(message.toUtf8());
        std::cout << "send CobotInit" << std::endl;
    }

    bool Client::BaseSpeedChange(double speed) {
        speed = std::clamp(speed, 0.0, 1.0);
        const QString message = QString("sdw default_speed %1").arg(speed);
        cmd_socket_->write(message.toUtf8());
        //std::cout << "speed control : " << speed << std::endl;
        return cmd_socket_->waitForBytesWritten();
    }

    void Client::ProgramMode_Real() {
        const QString message = "pgmode real";
        cmd_socket_->write(message.toUtf8());
        //std::cout << "send ProgramMode_Real " << std::endl;
    }

    void Client::MoveJ(double* joint, int speed, int acceleration) {
        QString message = QString("move_j(jnt[%1, %2, %3, %4, %5, %6], %7, %8)")
                          .arg(joint[0]).arg(joint[1]).arg(joint[2])
                          .arg(joint[3]).arg(joint[4]).arg(joint[5])
                          .arg(speed).arg(acceleration);
        cmd_socket_->write(message.toUtf8().constData(), message.toUtf8().size());
    }

    void Client::MoveL(double* pose, int speed, int acceleration) {
        QString message = QString("move_l(pnt[%1, %2, %3, %4, %5, %6], %7, %8)")
                          .arg(pose[0]).arg(pose[1]).arg(pose[2])
                          .arg(pose[3]).arg(pose[4]) .arg(pose[5])
                          .arg(speed).arg(acceleration);
        cmd_socket_->write(message.toUtf8().constData(), message.toUtf8().size());
    }

    void Client::SetDigitalOut(int port, int signal) {
        QString message = QString("set_box_dout(%1, %2)").arg(port).arg(signal);
        cmd_socket_->write(message.toUtf8());
    }

    void Client::SetDigitalOutAll(int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7,
                               int d8, int d9, int d10, int d11, int d12, int d13, int d14, int d15) {
        QString message = QString("digital_out %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15, %16")
                            .arg(d0).arg(d1).arg(d2).arg(d3)
                            .arg(d4).arg(d5).arg(d6).arg(d7)
                            .arg(d8).arg(d9).arg(d10).arg(d11)
                            .arg(d12).arg(d13).arg(d14).arg(d15);
        cmd_socket_->write(message.toUtf8());
    }

    void Client::SetToolOutAll(int volt, int d0, int d1) {
        if((volt != 12) && (volt != 24))
            volt = 0;
        QString message = QString("tool_out %1, %2, %3").arg(volt).arg(d0).arg(d1);
        cmd_socket_->write(message.toUtf8());
    }

    void Client::MotionPause() {
        const QString text = "task pause";
        cmd_socket_->write(text.toUtf8());
    }

    void Client::MotionResume() {
        const QString text = "task resume_a";
        cmd_socket_->write(text.toUtf8());
    }

    void Client::MotionStop() {
        const QString text = "task stop";
        cmd_socket_->write(text.toUtf8());
    }

    void Client::CollisionResume() {
        const QString text = "task resume_b";
        cmd_socket_->write(text.toUtf8());
    }
    // void Client::MakePose() {
    //     std::string pose;
    //     std::ifstream file("../settings/nexus_hot.wsl");
    //     if (file.is_open()) {
    //         while (getline(file, pose)) {
    //             if (pose.find("point")) {
    //                 std::string pose_key;
    //                 size_t pos1 = pose.find('(');
    //                 size_t pos2 = pose.find(')', pos1);
    //                 if (pos1 != std::string::npos && pos2 != std::string::npos) {
    //                     pose_key = pose.substr(pos1 + 1, pos2 - pos1 - 1);
    //                 }
    //
    //                 std::unordered_map<std::string, std::tuple<std::vector<double>, int>>::iterator find_iter;
    //
    //                 find_iter = pose_data_.find(pose_key);
    //                 if (find_iter != pose_data_.end()) {
    //                     //std::cout << "Exist Value : " << pose_key << std::endl;
    //                     ;
    //                 }
    //                 else {
    //                     //std::cout << "New Value" << std::endl;
    //                     double value;
    //                     size_t value1 = pose.find("absolute");
    //                     size_t value2 = pose.find(',', value1);
    //                     if (value1 != std::string::npos && value2 != std::string::npos) {
    //                         std::istringstream(pose.substr(value1 + 8, value2 - value1 - 8)) >> value;
    //                         value *= 1000;
    //                         int speed = static_cast<int>(value);
    //
    //                         std::vector<double> location;
    //                         std::istringstream ss(pose.substr(value2 + 6));
    //                         std::string token;
    //                         int cnt = 0;
    //                         while (std::getline(ss, token, ',') && cnt < 6) {
    //                             double number;
    //                             std::istringstream(token) >> number;
    //                             location.push_back(number);
    //                             ++cnt;
    //                         }
    //
    //                         pose_data_.insert({ pose_key, std::make_tuple(location, speed) });
    //                         pose_data_[pose_key] = std::make_tuple(location, speed);
    //                     }
    //                 }
    //             }
    //         }
    //     //print unordered_map
    //     // for (const auto& pair : pose_data_) {
    //     //     std::cout << "Key: " << pair.first << ", location: [";
    //     //     const auto& vec = std::get<0>(pair.second);
    //     //     std::cout << "Vector: [";
    //     //     for (const auto& v : vec) {
    //     //         std::cout << v << ", ";
    //     //     }
    //     //     std::cout << "], ";
    //     //     std::cout << "speed: " << std::get<1>(pair.second) << std::endl;
    //     // }
    //     //test
    //     // const double* ptr1 = std::get<0>(pose_data_["home"]).data();
    //     // int a = std::get<1>(pose_data_["home"]);
    //     // std::cout << "array : " << ptr1[0] << std::endl;;
    //     // std::cout << "int : " << a << std::endl;;
    //     }
    //
    //     file.close();
    //     std::cout << "file close " << std::endl;
    // }

    // double* Client::Position(std::string cobot_pose) {
    //     return std::get<0>(pose_data_[cobot_pose]).data();
    // }
    //
    // double Client::GetDistance(const double* pose) { // point_dist(P, P)로 대체
    //     double distance = std::sqrt(std::inner_product(systemStat_.sdata.tcp_ref,
    //                                                     systemStat_.sdata.tcp_ref + 3,
    //                                                     pose, 0.0,
    //                                                     std::plus<double>(),
    //                                                     [](double a, double b) {
    //                                                         return std::pow(std::abs(a - b), 2);
    //                                                     }));
    //     return distance;
    // }
    //
    // bool Client::IsHomePosition() {
    //     return (GetDistance(Position("home_l")) < 3) ? true : false;
    // }
}
