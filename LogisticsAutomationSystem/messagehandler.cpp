#include "messagehandler.h"

#include <QMessageBox>

MessageHandler::MessageHandler()
{

}

void MessageHandler::showOpenFileError(QString fileName)
{
    QMessageBox::critical(nullptr, "Помилка відкриття файлу",
                              "Не вдалося відкрити файл. "
                              "Перевірте, що файл " + fileName +
                              " розташований у папці програми.");
}

void MessageHandler::showOpenDatabaseError(QString databaseError)
{
    QMessageBox::critical(nullptr, "Помилка підключення бази данных",
                              "Не вдалося підключитися до бази даних. "
                              "Перевірте налаштування підключення! " +
                              databaseError);
}

void MessageHandler::showParcelIsArrangedInfo(int numberOrder, int costOrder)
{
    QString informationOrder = "Номер відправлення: " + QString::number(numberOrder) +
            "\n\nВартість доставки: " + QString::number(costOrder) + " грн";

    QMessageBox boxOrder(QMessageBox::Information, "Відправлення №" +
                         QString::number(numberOrder), "Відправлення успішно сформовано!");

    boxOrder.setInformativeText(informationOrder);
    boxOrder.setWindowIcon(QIcon(":/resources/resources/images/box.png"));
    boxOrder.exec();
}

void MessageHandler::showFailedParcelArrangedWarning()
{
    QMessageBox::warning(nullptr, "Помилка оформлення відправлення",
                              "Не вдалося підключитися до бази даних. "
                              "Перевірте налаштування підключення або спробуйте ще раз!");
}
