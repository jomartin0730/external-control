#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow),
                                          client_(new rb::Client),
                                          progress_bar(new QProgressBar) {
    ui->setupUi(this);
    ui->LE_Ip->setText("192.168.0.202");
    progress_bar->setRange(0, 100); // 진행률 범위 설정
    /*
     *Push Button Connect
     */
    connect(ui->PB_CobotInit, &QPushButton::clicked, this, [=] { client_->CobotInit(); });
    connect(ui->PB_CmdConnect, &QPushButton::clicked, this, &MainWindow::CmdConnected);
    connect(ui->PB_DataConnect, &QPushButton::clicked, this, &MainWindow::DataConnected);
    connect(ui->PB_MOTION_PAUSE, &QPushButton::clicked, this, &MainWindow::OnMotionPause);
    connect(ui->PB_MOTION_RESUME, &QPushButton::clicked, this, &MainWindow::OnMotionResume);
    connect(ui->PB_COLLISION_RESUME, &QPushButton::clicked, this, &MainWindow::OnCollisionResume);
    connect(ui->PB_MOTION_HALT, &QPushButton::clicked, this, &MainWindow::OnMotionStop);
    /*
     *Timer Connect
     */
    connect(&timer_, &QTimer::timeout, this, &MainWindow::RequestData);
    connect(&update_timer_, &QTimer::timeout, this, &MainWindow::UpdateSystem);
    update_timer_.start(100);
}

MainWindow::~MainWindow() {
    update_timer_.stop();
    delete ui;
}

void MainWindow::UpdateSystem() // 람다함수로 변경
{
    //robot_->MakePose();

    switch(systemStat_->sdata.init_state_info){
    case INIT_STAT_INFO_VOLTAGE_CHECK:
        ui->PGB_Init->setValue(progress += 15);
        break;
    case INIT_STAT_INFO_DEVICE_CHECK:
        ui->PGB_Init->setValue(progress += 15);
        break;
    case INIT_STAT_INFO_FIND_HOME:
        ui->PGB_Init->setValue(progress += 15);
        break;
    case INIT_STAT_INFO_VARIABLE_CHECK:
        ui->PGB_Init->setValue(progress += 15);
        break;
    case INIT_STAT_INFO_COLLISION_ON:
        ui->PGB_Init->setValue(progress += 20);
        break;
    case INIT_STAT_INFO_INIT_DONE:
        ui->PGB_Init->setValue(100);
        progress = 0;
        client_->ProgramMode_Real();
        client_->SetDigitalOutAll(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        client_->SetToolOutAll(24, 0, 0);
        //std::cout << "Cobot INIT Success" << std::endl;
        break;
    case INIT_STAT_INFO_NOACT:
        ui->PGB_Init->setValue(0);
        //std::cout << "Cobot INIT Error" << std::endl;
        break;
    default:
        break;
    }

    float speed = static_cast<float>(ui->HS_BASE_SPEED->value()) / 100.0f;
    float def_speed = systemStat_->sdata.default_speed;

    if (std::fabs(speed - def_speed) > 0.005f) {
        if (client_->BaseSpeedChange(speed))
            setBaseSpeed(speed);
    }
    else {
        ui->HS_BASE_SPEED->setValue(static_cast<int>(def_speed * 100));
    }

    //std::cout << "out : " << systemStat_->sdata.init_state_info << std::endl;
}

void MainWindow::RequestData(){
    client_->data_socket_->write("reqdata");
}

// connect & disconnect ------------------
void MainWindow::CmdConnected() {
    if (ui->PB_CmdConnect->text() == "Connect") {
        ui->PB_CmdConnect->setText("Disconnect");
        client_->TryConnect(client_->cmd_socket_, ui->LE_Ip->text(), ui->LE_CmdPort->text().toInt());
        connect(client_->cmd_socket_, &QTcpSocket::readyRead, client_, &rb::Client::CommandRead);
    }
    else {
        ui->PB_CmdConnect->setText("Connect");
        client_->cmd_socket_->close();
    }
    std::cout << "COMMAND " << (ui->PB_CmdConnect->text() == "Disconnect" ? " Connected" : " Disconnected") << std::endl;
}

void MainWindow::DataConnected() {
    if (ui->PB_DataConnect->text() == "Connect") {
        ui->PB_DataConnect->setText("Disconnect");
        client_->TryConnect(client_->data_socket_, ui->LE_Ip->text(), ui->LE_DataPort->text().toInt());
        connect(client_->data_socket_, &QTcpSocket::readyRead, client_, &rb::Client::DataRead);
        timer_.start(100);
    }
    else {
        ui->PB_DataConnect->setText("Connect");
        timer_.stop();
        client_->data_socket_->close();
    }
    std::cout << "DATA " << (ui->PB_DataConnect->text() == "Disconnect" ? " Connected" : " Disconnected") << std::endl;
}

void MainWindow::setBaseSpeed(float value) {
    ui->LB_BASE_SPEED->setText(QString().sprintf("%.1f%%", value*100.0));
}

void MainWindow::OnMotionPause() {
    client_->MotionPause();
}

void MainWindow::OnMotionResume() {
    client_->MotionResume();
}

void MainWindow::OnMotionStop() {
    client_->MotionStop();
}

void MainWindow::OnCollisionResume() {
    client_->CollisionResume();
}

void MainWindow::print(QString value) {
    QString str = ui->textPrint->toPlainText();
    ui->textPrint->setText(str+value);
    QScrollBar *sb = ui->textPrint->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}
