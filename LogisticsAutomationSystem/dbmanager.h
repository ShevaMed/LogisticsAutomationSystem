#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlTableModel>

class DBManager
{
public:
    static DBManager& instance();
    QSqlDatabase& getDatabase();
    QSqlTableModel* getTypeModel();
    QSqlTableModel* getDepartmentsModel();
    QSqlTableModel* getOrdersModel();
    QSqlTableModel* getUsersModel();

private:
    DBManager();
    ~DBManager();

    DBManager(const DBManager&) = delete;
    void operator=(const DBManager&) = delete;

public:
    int arrangeParcel(int cost, QString senderCity, QString recieverCity,
                       QString fullNameSender, QString fullNameReciever,
                       QString phoneSender, QString phoneReciever, QString type,
                       double weight, double length, double width, double height,
                       int costInsurance, bool recieverPays, bool packing,
                       QString parcelPays);

private:
    QSqlDatabase database;
    QSqlTableModel *typeModel;
    QSqlTableModel *departmentsModel;
    QSqlTableModel *ordersModel;
    QSqlTableModel *usersModel;

public:
    static int userId;
};

#endif // DBMANAGER_H
