#include "mainwindow.h"
#include "database.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 数据库文件放在可执行文件同目录，便于用 Navicat / DB Browser 直接打开查看
    const QString dbPath = QDir(QCoreApplication::applicationDirPath()).filePath("hospital.db");

    if (!Database::instance().open(dbPath)) {
        // 数据库连接失败：给出明确提示（包含路径与底层错误信息）
        QMessageBox::critical(nullptr, "数据库连接失败",
            "无法打开数据库：\n" + dbPath +
            "\n\n错误信息：\n" + Database::instance().lastError());
        return 1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
