#include "mainwindow.h"
#include "ui_mainwindow.h"

#define EARTH_RADIUS 6371

#include <QMessageBox>
#include <QSqlRecord>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionLogout->setDisabled(true);
    ui->actionShowDepartments->setDisabled(true);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./DB.db");

    if (db.open())
    {
        qDebug("open");

        //---TypeOfDispatch-------------------------------------------------------------------------------------
        modelType = new QSqlTableModel(this, db);
        modelType->setTable("TypeOfDispatch");
        modelType->select();

        ui->arrangeTypeComboBox->setModel(modelType);
        ui->arrangeTypeComboBox->setModelColumn(1);

        ui->calcTypeComboBox->setModel(modelType);
        ui->calcTypeComboBox->setModelColumn(1);

        //---Departments-----------------------------------------------------------------------------------------
        modelDepartments = new QSqlTableModel(this, db);
        modelDepartments->setTable("Departments");
        modelDepartments->select();

        modelDepartments->setHeaderData(1, Qt::Horizontal, "Місцезнаходження", Qt::DisplayRole);
        modelDepartments->setHeaderData(2, Qt::Horizontal, "Географічна Широта", Qt::DisplayRole);
        modelDepartments->setHeaderData(3, Qt::Horizontal, "Географічна Довгота", Qt::DisplayRole);

        ui->arrangeCitySenderComboBox->setModel(modelDepartments);
        ui->arrangeCitySenderComboBox->setModelColumn(1);
        ui->arrangeCityRecieverComboBox->setModel(modelDepartments);
        ui->arrangeCityRecieverComboBox->setModelColumn(1);

        ui->calcCitySenderComboBox->setModel(modelDepartments);
        ui->calcCitySenderComboBox->setModelColumn(1);
        ui->calcCityRecieverComboBox->setModel(modelDepartments);
        ui->calcCityRecieverComboBox->setModelColumn(1);

        ui->departmentsTableView->setModel(modelDepartments);
        ui->departmentsTableView->horizontalHeader()
                ->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->departmentsTableView->setColumnHidden(0, true);

        //---Orders------------------------------------------------------------------------------------------------
        modelOrders = new QSqlTableModel(this, db);
        modelOrders->setTable("Orders");
        modelOrders->select();

        modelOrders->setHeaderData(0, Qt::Horizontal, "№ Відправлення", Qt::DisplayRole);
        modelOrders->setHeaderData(1, Qt::Horizontal, "Статус відправлення", Qt::DisplayRole);
        modelOrders->setHeaderData(2, Qt::Horizontal, "Дата відправлення", Qt::DisplayRole);
        modelOrders->setHeaderData(3, Qt::Horizontal, "Вартість", Qt::DisplayRole);
        modelOrders->setHeaderData(4, Qt::Horizontal, "Місто-відправник", Qt::DisplayRole);
        modelOrders->setHeaderData(5, Qt::Horizontal, "Місто-отримувач", Qt::DisplayRole);
        modelOrders->setHeaderData(6, Qt::Horizontal, "ПІБ відправника", Qt::DisplayRole);
        modelOrders->setHeaderData(7, Qt::Horizontal, "ПІБ отримувача", Qt::DisplayRole);
        modelOrders->setHeaderData(8, Qt::Horizontal, "Телефон відправника", Qt::DisplayRole);
        modelOrders->setHeaderData(9, Qt::Horizontal, "Телефон отримувача", Qt::DisplayRole);
        modelOrders->setHeaderData(10, Qt::Horizontal, "Тип відправлення", Qt::DisplayRole);
        modelOrders->setHeaderData(11, Qt::Horizontal, "Вага", Qt::DisplayRole);
        modelOrders->setHeaderData(12, Qt::Horizontal, "Довжина", Qt::DisplayRole);
        modelOrders->setHeaderData(13, Qt::Horizontal, "Ширина", Qt::DisplayRole);
        modelOrders->setHeaderData(14, Qt::Horizontal, "Висота", Qt::DisplayRole);
        modelOrders->setHeaderData(15, Qt::Horizontal, "Сума страхування", Qt::DisplayRole);
        modelOrders->setHeaderData(16, Qt::Horizontal, "Отримувач сплачює суму страхування",
                                   Qt::DisplayRole);
        modelOrders->setHeaderData(17, Qt::Horizontal, "Пакування", Qt::DisplayRole);
        modelOrders->setHeaderData(18, Qt::Horizontal, "Доставку сплачює", Qt::DisplayRole);

        ui->searchShowAllTableView->setModel(modelOrders);
        ui->searchShowAllTableView->horizontalHeader()
                ->setSectionResizeMode(QHeaderView::ResizeToContents);

        ui->statusShowChangeTableView->setModel(modelOrders);
        ui->statusShowChangeTableView->horizontalHeader()
                ->setSectionResizeMode(QHeaderView::ResizeToContents);

        //---UsersInfo-----------------------------------------------------------------------------------------------
        modelUsers = new QSqlTableModel(this, db);
        modelUsers->setTable("UsersInfo");
        modelUsers->select();
    }
    else
    {
        qDebug("no open");
    }

    ui->calcCityRecieverComboBox->setCurrentIndex(-1);
    ui->calcCitySenderComboBox->setCurrentIndex(-1);
    ui->arrangeCityRecieverComboBox->setCurrentIndex(-1);
    ui->arrangeCitySenderComboBox->setCurrentIndex(-1);

}

MainWindow::~MainWindow()
{
    db.close();
    delete ui;
}


double MainWindow::calculateTheDistanceOnTheEarth(double latitudeA, double longitudeA,
                                                  double latitudeB, double longitudeB)
{
    // перевести координаты в радианы
    double latA = latitudeA * M_PI / 180;
    double latB = latitudeB * M_PI / 180;
    double longA = longitudeA * M_PI / 180;
    double longB = longitudeB * M_PI / 180;

    // косинусы и синусы широт и разницы долгот
    double cos_latA = cos(latA);
    double cos_latB = cos(latB);
    double sin_latA = sin(latA);
    double sin_latB = sin(latB);
    double delta = longB - longA;
    double cos_delta = cos(delta);
    double sin_delta = sin(delta);

    // вычисления длины большого круга
    double x = sin_latA * sin_latB + cos_latA * cos_latB * cos_delta;
    double y = sqrt((cos_latB * sin_delta) * (cos_latB * sin_delta) +
                    (cos_latA * sin_latB - sin_latA * cos_latB * cos_delta) *
                    (cos_latA * sin_latB - sin_latA * cos_latB * cos_delta));

    return atan2(y, x) * EARTH_RADIUS;
}


double MainWindow::calculateCost(int senderDepartmentIndex, int recieverDepartmentIndex,
                                 double length, double width, double height, double weight,
                                 int costInsurance, bool packing, int amount)
{
    double senderLatitude = modelDepartments->record(senderDepartmentIndex)
            .value("GeographicalLatitude").toDouble();
    double senderLongitude = modelDepartments->record(senderDepartmentIndex)
            .value("GeographicalLongitude").toDouble();
    double recieverLatitude = modelDepartments->record(recieverDepartmentIndex)
            .value("GeographicalLatitude").toDouble();
    double recieverLongitude = modelDepartments->record(recieverDepartmentIndex)
            .value("GeographicalLongitude").toDouble();

    double distance = calculateTheDistanceOnTheEarth(senderLatitude, senderLongitude,
                                                     recieverLatitude, recieverLongitude);
    double volume = (length * width * height) / 1000;
    double profile = (volume > weight) ? volume : weight;

    double cost = 50 + distance * 0.1 + profile * 5 + costInsurance * 0.005;

    if(packing)
        cost += volume * 8;

    cost *= amount;

    return cost;
}


void MainWindow::on_arrangeParcelButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->arrangeParcelWidget);
    ui->statusBar->clearMessage();
}


void MainWindow::on_calculateButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->calculateWidget);
    ui->statusBar->clearMessage();
}


void MainWindow::on_calcCalculatePushButton_clicked()
{
    ui->calcCostLCDNumber->display(0);

    int senderDepartmentIndex = ui->calcCitySenderComboBox->currentIndex();
    int recieverDepartmentIndex = ui->calcCityRecieverComboBox->currentIndex();

    if (senderDepartmentIndex == -1 || recieverDepartmentIndex == -1)
    {
        ui->statusBar->showMessage("Введіть маршрут!");
        return;
    }

    if (senderDepartmentIndex == recieverDepartmentIndex)
    {
        ui->statusBar->showMessage("Некоректний маршрут! Міста повинні бути різними");
        return;
    }

    QString type = ui->calcTypeComboBox->currentText();
    double length = 0.0, width = 0.0, height = 0.0, weight = 0.0;

    if (type == "Посилка")
    {
        length = ui->calcLengthLineEdit->text().toDouble();
        width = ui->calcWidthLineEdit->text().toDouble();
        height = ui->calcHeightLineEdit->text().toDouble();
        weight = ui->calcWeightLineEdit->text().toDouble();

        if (weight == 0.0)
        {
            ui->statusBar->showMessage("Введіть коректну вагу відправлення!");
            return;
        }

        if (length == 0.0 || width == 0.0 || height == 0.0)
        {
            ui->statusBar->showMessage("Введіть коректно всі розміри відправлення!");
            return;
        }
    }

    if (type == "Документи")
    {
        length = 30;
        width = 23;
        height = 2;
        weight = 0.2;
    }

    int costInsurance = ui->calcCostInsuranceLineEdit->text().toInt();

    if (costInsurance == 0)
    {
        ui->statusBar->showMessage("Введіть коректну ціну страхування!");
        return;
    }

    bool packing = ui->calcPackingCheckBox->isChecked();
    int amount = ui->calcAmountSpinBox->value();

    int cost = calculateCost(senderDepartmentIndex, recieverDepartmentIndex,
                             length, width, height, weight, costInsurance, packing, amount);

    ui->calcCostLCDNumber->display(cost);

    ui->statusBar->showMessage("Розраховано!");
}


void MainWindow::on_statusShowChangeButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->statusShowChangeWidget);
    ui->statusBar->clearMessage();

    on_statusInNumberLineEdit_textChanged(ui->statusInNumberLineEdit->text());
}


void MainWindow::on_searchShowAllButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->searchShowAllWidget);
    ui->statusBar->clearMessage();

    on_searchSearchLineEdit_textChanged(ui->searchSearchLineEdit->text());
}


void MainWindow::on_arrangeArrangePushButton_clicked()
{
    int senderDepartmentIndex = ui->arrangeCitySenderComboBox->currentIndex();
    int recieverDepartmentIndex = ui->arrangeCityRecieverComboBox->currentIndex();

    if (senderDepartmentIndex == -1 || recieverDepartmentIndex == -1)
    {
        ui->statusBar->showMessage("Введіть маршрут!");
        return;
    }

    if (senderDepartmentIndex == recieverDepartmentIndex)
    {
        ui->statusBar->showMessage("Некоректний маршрут! Міста повинні бути різними");
        return;
    }

    QString FullNameSender = ui->arrangeSenderFullNameLineEdit->text();
    QString FullNameReciever = ui->arrangeRecieverFullNameLineEdit->text();

    if (FullNameSender == "")
    {
        ui->statusBar->showMessage("Введіть ПІБ відправника!");
        return;
    }

    if (FullNameReciever == "")
    {
        ui->statusBar->showMessage("Введіть ПІБ отримувача!");
        return;
    }

    QString PhoneSender = ui->arrangeSenderPhoneLineEdit->text();
    QString PhoneReciever = ui->arrangeRecieverPhoneLineEdit->text();

    if (PhoneSender.length() != 19)
    {
        ui->statusBar->showMessage("Введіть коректний телефон відправника!");
        return;
    }

    if (PhoneReciever.length() != 19)
    {
        ui->statusBar->showMessage("Введіть коректний телефон отримувача!");
        return;
    }

    QString type = ui->arrangeTypeComboBox->currentText();
    double length = 0.0, width = 0.0, height = 0.0, weight = 0.0;

    if (type == "Посилка")
    {
        length = ui->arrangeLengthLineEdit->text().toDouble();
        width = ui->arrangeWidthLineEdit->text().toDouble();
        height = ui->arrangeHeightLineEdit->text().toDouble();
        weight = ui->arrangeWeightLineEdit->text().toDouble();

        if (weight == 0.0)
        {
            ui->statusBar->showMessage("Введіть коректну вагу відправлення!");
            return;
        }

        if (length == 0.0 || width == 0.0 || height == 0.0)
        {
            ui->statusBar->showMessage("Введіть коректно всі розміри відправлення!");
            return;
        }
    }

    if (type == "Документи")
    {
        length = 30;
        width = 23;
        height = 2;
        weight = 0.2;
    }

    int costInsurance = ui->arrangeCostInsuranceLineEdit->text().toInt();

    if (costInsurance == 0)
    {
        ui->statusBar->showMessage("Введіть коректну ціну страхування!");
        return;
    }

    bool packing = ui->arrangePackingCheckBox->isChecked();
    int amount = ui->arrangeAmountSpinBox->value();

    int cost = calculateCost(senderDepartmentIndex, recieverDepartmentIndex,
                             length, width, height, weight, costInsurance, packing, amount);

    ui->statusBar->clearMessage();

    int rowIndex = modelOrders->rowCount();
    modelOrders->insertRow(rowIndex);

    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("Key")),
                         rowIndex + 1);
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("StatusParcel")),
                         "Комплектується");
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("DateOfDispatch")),
                         QDateTime::currentDateTime().toString());
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("Cost")),
                         cost);
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("SenderСity")),
                         ui->arrangeCitySenderComboBox->currentText());
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("RecieverCity")),
                         ui->arrangeCityRecieverComboBox->currentText());
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("SenderFullName")),
                         FullNameSender);
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("RecieverFullName")),
                         FullNameReciever);
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("SenderPhone")),
                         PhoneSender);
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("RecieverPhone")),
                         PhoneReciever);
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("Type")),
                         type);
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("Weight")),
                         weight);
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("Length")),
                         length);
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("Width")),
                         width);
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("Height")),
                         height);
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("CostOfInsurance")),
                         costInsurance);
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("RecieverPays")),
                         ui->arrangeRecieverPayCheckBox->isChecked());
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("Packaging")),
                         ui->arrangePackingCheckBox->isChecked());
    modelOrders->setData(modelOrders
                         ->index(rowIndex, modelOrders->fieldIndex("ParcelPays")),
                         ui->arrangeParcelPayComboBox->currentText());

    modelOrders->submitAll();

    QString numberOrder = std::to_string(rowIndex + 1).c_str();
    QString costOrder = std::to_string(cost).c_str();

    QString informationOrder = "Номер відправлення: " + numberOrder +
            "\n\nВартість доставки: " + costOrder + " грн";

    QMessageBox boxOrder(QMessageBox::Information, "Відправлення №" +
                         numberOrder, "Відправлення успішно сформовано!");

    boxOrder.setInformativeText(informationOrder);
    boxOrder.setWindowIcon(QIcon(":/resources/resources/images/box.png"));
    boxOrder.exec();

    ui->statusBar->showMessage("Відправлення №" + numberOrder + " успішно сформовано!");
    ui->stackedWidget->setCurrentWidget(ui->mainMenuWidget);

    resetDefaultArrangeWidget();
}


void MainWindow::on_searchShowAllpushButton_clicked()
{
    ui->searchSearchLineEdit->clear();
    modelOrders->setFilter("");
}


void MainWindow::on_searchSearchLineEdit_textChanged(const QString &arg1)
{
    modelOrders->setFilter("");

    if (arg1 == "")
    {
        ui->statusBar->showMessage("Показані всі відправлення");
    }
    else
    {
        if (!modelOrders->record(arg1.toInt() - 1).value("Key").isNull())
        {
            modelOrders->setFilter("Key = " + arg1);
            ui->searchShowAllTableView->selectRow(0);

            ui->statusBar->showMessage("Відправлення №" + arg1 + " знайдено!");
        }
        else
        {
            modelOrders->setFilter("Key = 0");
            ui->statusBar->showMessage("Відправлення №" + arg1 + " не знайдено!");
        }
    }
}


void MainWindow::on_statusInNumberLineEdit_textChanged(const QString &arg1)
{
    ui->statusStatusShowLabel->clear();
    ui->statusProgressBar->setValue(0);
    ui->statusStatusChangeComboBox->setCurrentIndex(-1);
    ui->statusStatusChangeComboBox->setDisabled(true);

    if (arg1 == "")
    {
        modelOrders->setFilter("Key = 0");
        ui->statusBar->clearMessage();
    }
    else
    {
        modelOrders->setFilter("");
        QString status = modelOrders->record(arg1.toInt() - 1).value("StatusParcel").toString();

        if (status != "")
        {
            modelOrders->setFilter("Key = " + arg1);
            ui->statusShowChangeTableView->selectRow(0);

            ui->statusStatusShowLabel->setText(status);
            ui->statusStatusChangeComboBox->setEnabled(true);
            ui->statusStatusChangeComboBox
                    ->setCurrentIndex(ui->statusStatusChangeComboBox->findText(status));

            int progress = 0;
            if (status == "Комплектується")
                progress = 20;
            else if (status == "В дорозі")
                progress = 65;
            else if (status == "Доставлено")
                progress = 100;

            ui->statusProgressBar->setValue(progress);
            ui->statusBar->showMessage("Відправлення №" + arg1 + " знайдено!");
        }
        else
        {
            modelOrders->setFilter("Key = 0");
            ui->statusBar->showMessage("Відправлення №" + arg1 + " не знайдено!");
        }
    }
}


void MainWindow::on_statusStatusChangeComboBox_currentTextChanged(const QString &arg1)
{
    modelOrders->setData(modelOrders->index(0, modelOrders->fieldIndex("StatusParcel")), arg1);
    modelOrders->submitAll();

    ui->statusStatusShowLabel->setText(arg1);

    int progress = 0;
    if (arg1 == "Комплектується")
        progress = 20;
    else if (arg1 == "В дорозі")
        progress = 65;
    else if (arg1 == "Доставлено")
        progress = 100;

    ui->statusProgressBar->setValue(progress);

    QString numberOrder = modelOrders->record(0).value("Key").toString();
    ui->statusBar->showMessage("Статус відправлення №" + numberOrder + " змінено!");
}


void MainWindow::on_calcMainMenuCommandLinkButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->mainMenuWidget);
    ui->statusBar->clearMessage();
}


void MainWindow::on_arrangeMainMenuCommandLinkButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->mainMenuWidget);
    ui->statusBar->clearMessage();
}


void MainWindow::on_statusMainMenuCommandLinkButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->mainMenuWidget);
    ui->statusBar->clearMessage();
}


void MainWindow::on_searchMainMenuCommandLinkButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->mainMenuWidget);
    ui->statusBar->clearMessage();
}


void MainWindow::on_actionExit_triggered()
{
    close();
}


void MainWindow::on_actionLogout_triggered()
{
    ui->passwordLineEdit->clear();
    ui->stackedWidget->setCurrentWidget(ui->loginWidget);

    ui->actionLogout->setDisabled(true);
    ui->actionShowDepartments->setDisabled(true);

    resetDefaultArrangeWidget();
    resetDefaultCalculateWidget();
    ui->statusInNumberLineEdit->clear();
    ui->searchSearchLineEdit->clear();

    ui->statusBar->clearMessage();
}


void MainWindow::on_loginPushButton_clicked()
{
    int usersCount = modelUsers->rowCount();

    for (int i = 0; i < usersCount; i++)
    {
        QString login = modelUsers->record(i).value("Login").toString();
        QString password = modelUsers->record(i).value("Password").toString();

        if (login == ui->loginLineEdit->text())
        {
            if (password == ui->passwordLineEdit->text())
            {
                QString name = modelUsers->record(i).value("FullName").toString();
                ui->mainGreetingLabel->setText("Обліковий запис: " + name);

                ui->stackedWidget->setCurrentWidget(ui->mainMenuWidget);
                ui->statusBar->showMessage("Успішний вхід!");

                ui->actionLogout->setEnabled(true);
                ui->actionShowDepartments->setEnabled(true);
                return;
            }
            else
            {
                ui->statusBar->showMessage("Невірний пароль!");
                ui->passwordLineEdit->clear();
                return;
            }
        }
    }

    ui->statusBar->showMessage("Недійсний логін!");
}


void MainWindow::on_actionShowDepartments_triggered()
{
     ui->stackedWidget->setCurrentWidget(ui->departmentsWidget);
     ui->statusBar->showMessage("Показані всі відділення по Україні");
}


void MainWindow::on_departmentsMainMenuCommandLinkButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->mainMenuWidget);
    ui->statusBar->clearMessage();
}


void MainWindow::on_calcTypeComboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 == "Посилка")
    {
        ui->calcWeightLineEdit->setEnabled(true);
        ui->calcWidthLineEdit->setEnabled(true);
        ui->calcLengthLineEdit->setEnabled(true);
        ui->calcHeightLineEdit->setEnabled(true);
    }

    if (arg1 == "Документи")
    {
        ui->calcWeightLineEdit->clear();
        ui->calcWidthLineEdit->clear();
        ui->calcLengthLineEdit->clear();
        ui->calcHeightLineEdit->clear();

        ui->calcWeightLineEdit->setDisabled(true);
        ui->calcWidthLineEdit->setDisabled(true);
        ui->calcLengthLineEdit->setDisabled(true);
        ui->calcHeightLineEdit->setDisabled(true);
    }
}


void MainWindow::on_arrangeTypeComboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 == "Посилка")
    {
        ui->arrangeWeightLineEdit->setEnabled(true);
        ui->arrangeWidthLineEdit->setEnabled(true);
        ui->arrangeLengthLineEdit->setEnabled(true);
        ui->arrangeHeightLineEdit->setEnabled(true);
    }

    if (arg1 == "Документи")
    {
        ui->arrangeWeightLineEdit->clear();
        ui->arrangeWidthLineEdit->clear();
        ui->arrangeLengthLineEdit->clear();
        ui->arrangeHeightLineEdit->clear();

        ui->arrangeWeightLineEdit->setDisabled(true);
        ui->arrangeWidthLineEdit->setDisabled(true);
        ui->arrangeLengthLineEdit->setDisabled(true);
        ui->arrangeHeightLineEdit->setDisabled(true);
    }
}


void MainWindow::resetDefaultCalculateWidget()
{
    ui->calcCityRecieverComboBox->setCurrentIndex(-1);
    ui->calcCitySenderComboBox->setCurrentIndex(-1);
    ui->calcTypeComboBox->setCurrentIndex(0);

    ui->calcAmountSpinBox->setValue(1);
    ui->calcCostLCDNumber->display(0);
    ui->calcPackingCheckBox->setChecked(false);

    ui->calcCostInsuranceLineEdit->clear();
    ui->calcWeightLineEdit->clear();
    ui->calcWidthLineEdit->clear();
    ui->calcLengthLineEdit->clear();
    ui->calcHeightLineEdit->clear();
}


void MainWindow::resetDefaultArrangeWidget()
{
    ui->arrangeCityRecieverComboBox->setCurrentIndex(-1);
    ui->arrangeCitySenderComboBox->setCurrentIndex(-1);
    ui->arrangeTypeComboBox->setCurrentIndex(0);
    ui->arrangeParcelPayComboBox->setCurrentIndex(0);

    ui->arrangeAmountSpinBox->setValue(1);
    ui->arrangePackingCheckBox->setChecked(false);
    ui->arrangeRecieverPayCheckBox->setChecked(false);

    ui->arrangeSenderFullNameLineEdit->clear();
    ui->arrangeRecieverFullNameLineEdit->clear();
    ui->arrangeSenderPhoneLineEdit->clear();
    ui->arrangeRecieverPhoneLineEdit->clear();
    ui->arrangeCostInsuranceLineEdit->clear();
    ui->arrangeWeightLineEdit->clear();
    ui->arrangeWidthLineEdit->clear();
    ui->arrangeLengthLineEdit->clear();
    ui->arrangeHeightLineEdit->clear();
}


void MainWindow::on_calcClearToolButton_clicked()
{
    resetDefaultCalculateWidget();
    ui->statusBar->clearMessage();
}

