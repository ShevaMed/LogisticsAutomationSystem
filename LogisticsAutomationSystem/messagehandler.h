#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QString>

class MessageHandler
{
public:
    MessageHandler();

    static void showOpenFileError(QString fileName);
    static void showOpenDatabaseError(QString databaseError);
    static void showParcelIsArrangedInfo(int numberOrder, int costOrder);
    static void showFailedParcelArrangedWarning();
};

#endif // MESSAGEHANDLER_H
