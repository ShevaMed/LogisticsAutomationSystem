#include "dbmanager.h"
#include "messagehandler.h"

#include <QDateTime>

DBManager &DBManager::instance()
{
    static DBManager instance;
    return instance;
}

QSqlDatabase &DBManager::getDatabase()
{
    return database;
}

QSqlTableModel *DBManager::getTypeModel()
{
    if (!typeModel) {
        typeModel = new QSqlTableModel(nullptr, database);
        typeModel->setTable("TypeOfDispatch");
        typeModel->select();
    }
    return typeModel;
}

QSqlTableModel *DBManager::getDepartmentsModel()
{
    if (!departmentsModel) {
        departmentsModel = new QSqlTableModel(nullptr, database);
        departmentsModel->setTable("Departments");
        departmentsModel->select();
        departmentsModel->setHeaderData(1, Qt::Horizontal, "Місцезнаходження", Qt::DisplayRole);
        departmentsModel->setHeaderData(2, Qt::Horizontal, "Географічна Широта", Qt::DisplayRole);
        departmentsModel->setHeaderData(3, Qt::Horizontal, "Географічна Довгота", Qt::DisplayRole);
    }
    return departmentsModel;
}

QSqlTableModel *DBManager::getOrdersModel()
{
    if (!ordersModel) {
        ordersModel = new QSqlTableModel(nullptr, database);
        ordersModel->setTable("Orders");
        ordersModel->select();
        ordersModel->setHeaderData(0, Qt::Horizontal, "№ Відправлення", Qt::DisplayRole);
        ordersModel->setHeaderData(1, Qt::Horizontal, "Статус відправлення", Qt::DisplayRole);
        ordersModel->setHeaderData(2, Qt::Horizontal, "Дата відправлення", Qt::DisplayRole);
        ordersModel->setHeaderData(3, Qt::Horizontal, "Вартість", Qt::DisplayRole);
        ordersModel->setHeaderData(4, Qt::Horizontal, "Місто-відправник", Qt::DisplayRole);
        ordersModel->setHeaderData(5, Qt::Horizontal, "Місто-отримувач", Qt::DisplayRole);
        ordersModel->setHeaderData(6, Qt::Horizontal, "ПІБ відправника", Qt::DisplayRole);
        ordersModel->setHeaderData(7, Qt::Horizontal, "ПІБ отримувача", Qt::DisplayRole);
        ordersModel->setHeaderData(8, Qt::Horizontal, "Телефон відправника", Qt::DisplayRole);
        ordersModel->setHeaderData(9, Qt::Horizontal, "Телефон отримувача", Qt::DisplayRole);
        ordersModel->setHeaderData(10, Qt::Horizontal, "Тип відправлення", Qt::DisplayRole);
        ordersModel->setHeaderData(11, Qt::Horizontal, "Вага", Qt::DisplayRole);
        ordersModel->setHeaderData(12, Qt::Horizontal, "Довжина", Qt::DisplayRole);
        ordersModel->setHeaderData(13, Qt::Horizontal, "Ширина", Qt::DisplayRole);
        ordersModel->setHeaderData(14, Qt::Horizontal, "Висота", Qt::DisplayRole);
        ordersModel->setHeaderData(15, Qt::Horizontal, "Сума страхування", Qt::DisplayRole);
        ordersModel->setHeaderData(16, Qt::Horizontal, "Отримувач сплачює суму страхування",
                                   Qt::DisplayRole);
        ordersModel->setHeaderData(17, Qt::Horizontal, "Пакування", Qt::DisplayRole);
        ordersModel->setHeaderData(18, Qt::Horizontal, "Доставку сплачює", Qt::DisplayRole);
    }
    return ordersModel;
}

QSqlTableModel *DBManager::getUsersModel()
{
    if (!usersModel) {
        usersModel = new QSqlTableModel(nullptr, database);
        usersModel->setTable("UsersInfo");
        usersModel->select();
    }
    return usersModel;
}

DBManager::DBManager() :
    typeModel(nullptr), departmentsModel(nullptr),
    ordersModel(nullptr), usersModel(nullptr)
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("database.db");
}

int DBManager::arrangeParcel(int cost, QString senderCity, QString recieverCity,
                              QString fullNameSender, QString fullNameReciever,
                              QString phoneSender, QString phoneReciever, QString type,
                              double weight, double length, double width, double height,
                              int costInsurance, bool recieverPays, bool packing,
                              QString parcelPays)
{
    int rowIndex = ordersModel->rowCount();
    ordersModel->insertRow(rowIndex);
    int numberOrder = rowIndex + 1;

    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("Key")),
                         numberOrder);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("StatusParcel")),
                         "Комплектується");
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("DateOfDispatch")),
                         QDateTime::currentDateTime().toString());
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("Cost")),
                         cost);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("SenderСity")),
                         senderCity);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("RecieverCity")),
                         recieverCity);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("SenderFullName")),
                         fullNameSender);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("RecieverFullName")),
                         fullNameReciever);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("SenderPhone")),
                         phoneSender);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("RecieverPhone")),
                         phoneReciever);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("Type")),
                         type);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("Weight")),
                         weight);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("Length")),
                         length);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("Width")),
                         width);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("Height")),
                         height);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("CostOfInsurance")),
                         costInsurance);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("RecieverPays")),
                         recieverPays);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("Packaging")),
                         packing);
    ordersModel->setData(ordersModel
                         ->index(rowIndex, ordersModel->fieldIndex("ParcelPays")),
                         parcelPays);

    if (!ordersModel->submitAll()) {
        MessageHandler::showFailedParcelArrangedWarning();
        return -1;
    }

    MessageHandler::showParcelIsArrangedInfo(numberOrder, cost);
    return numberOrder;
}

DBManager::~DBManager()
{
    if (typeModel) {
        delete typeModel;
    }
    if (departmentsModel) {
        delete departmentsModel;
    }
    if (ordersModel) {
        delete ordersModel;
    }
    if (usersModel) {
        delete usersModel;
    }
    if (database.isOpen()) {
        database.close();
    }
}
