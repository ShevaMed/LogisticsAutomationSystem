#include "mainwindow.h"
#include "dbmanager.h"
#include "messagehandler.h"

#include <QApplication>
#include <QFile>
#include <QSqlError>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase& db = DBManager::instance().getDatabase();
    if (!db.open()) {
        MessageHandler::showOpenDatabaseError(db.lastError().text());
        return 1;
    }

    QString styleSheetFileName = "stylesheet.qss";
    QFile styleSheetFile(styleSheetFileName);
    if (!styleSheetFile.open(QFile::ReadOnly)) {
        MessageHandler::showOpenFileError(styleSheetFileName);
    } else {
        QString styleSheet = QLatin1String(styleSheetFile.readAll());
        a.setStyleSheet(styleSheet);
        styleSheetFile.close();
    }

    MainWindow w;
    w.show();

    return a.exec();
}
