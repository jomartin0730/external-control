#include "mainwindow.h"
#include <QApplication>
#include <QFont>
#include <QFontDatabase>

int main(int argc, char *argv[]) {
    /**
 * Font 설정
 */
    QApplication a(argc, argv);
    int fontId = QFontDatabase::addApplicationFont(":/fonts/PretendardVariable.ttf");
    if (fontId == -1) {
        qWarning() << "Failed to load the font!";
    }
    else {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont font(fontFamily, 10, 10);
        font.setStyleStrategy(QFont::PreferAntialias);
        a.setFont(font);  // Set the font as the application's default font (optional)
    }

    MainWindow w;
    w.resize(1920, 1080); // 메인 윈도우 크기 설정
    w.show();
    return a.exec();
}
