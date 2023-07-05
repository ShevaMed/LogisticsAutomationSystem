#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbmanager.h"
#include "calculation.h"

#include <QSqlRecord>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      departmentsModel_(DBManager::instance().getDepartmentsModel()),
      ordersModel_(DBManager::instance().getOrdersModel())
{
    ui->setupUi(this);

    ui->actionLogout->setDisabled(true);
    ui->actionShowDepartments->setDisabled(true);

    ui->arrangeTypeComboBox->setModel(DBManager::instance().getTypeModel());
    ui->arrangeTypeComboBox->setModelColumn(1);

    ui->calcTypeComboBox->setModel(DBManager::instance().getTypeModel());
    ui->calcTypeComboBox->setModelColumn(1);

    ui->arrangeCitySenderComboBox->setModel(departmentsModel_);
    ui->arrangeCitySenderComboBox->setModelColumn(1);
    ui->arrangeCityRecieverComboBox->setModel(departmentsModel_);
    ui->arrangeCityRecieverComboBox->setModelColumn(1);

    ui->calcCitySenderComboBox->setModel(departmentsModel_);
    ui->calcCitySenderComboBox->setModelColumn(1);
    ui->calcCityRecieverComboBox->setModel(departmentsModel_);
    ui->calcCityRecieverComboBox->setModelColumn(1);

    ui->departmentsTableView->setModel(departmentsModel_);
    ui->departmentsTableView->horizontalHeader()
            ->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->departmentsTableView->setColumnHidden(0, true);

    ui->searchShowAllTableView->setModel(ordersModel_);
    ui->searchShowAllTableView->horizontalHeader()
            ->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->statusShowChangeTableView->setModel(ordersModel_);
    ui->statusShowChangeTableView->horizontalHeader()
            ->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->calcCityRecieverComboBox->setCurrentIndex(-1);
    ui->calcCitySenderComboBox->setCurrentIndex(-1);
    ui->arrangeCityRecieverComboBox->setCurrentIndex(-1);
    ui->arrangeCitySenderComboBox->setCurrentIndex(-1);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    QString type = ui->calcTypeComboBox->currentText();
    double length = ui->calcLengthLineEdit->text().toDouble();
    double width = ui->calcWidthLineEdit->text().toDouble();
    double height = ui->calcHeightLineEdit->text().toDouble();
    double weight = ui->calcWeightLineEdit->text().toDouble();
    int costInsurance = ui->calcCostInsuranceLineEdit->text().toInt();

    if (!Calculation::defineParamsParcelDependOfType(ui->statusBar, type, senderDepartmentIndex,
                                                     recieverDepartmentIndex, length, width,
                                                     height, weight, costInsurance)) {
        return;
    }

    bool packing = ui->calcPackingCheckBox->isChecked();
    int amount = ui->calcAmountSpinBox->value();
    int cost = Calculation::calculateCost(senderDepartmentIndex, recieverDepartmentIndex,
                                          length, width, height, weight,
                                          costInsurance, packing, amount);
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
    QString fullNameSender = ui->arrangeSenderFullNameLineEdit->text();
    QString fullNameReciever = ui->arrangeRecieverFullNameLineEdit->text();

    if (fullNameSender.isEmpty()) {
        ui->statusBar->showMessage("Введіть ПІБ відправника!");
        return;
    }

    if (fullNameReciever.isEmpty()) {
        ui->statusBar->showMessage("Введіть ПІБ отримувача!");
        return;
    }

    QString phoneSender = ui->arrangeSenderPhoneLineEdit->text();
    QString phoneReciever = ui->arrangeRecieverPhoneLineEdit->text();

    if (phoneSender.length() != 19) {
        ui->statusBar->showMessage("Введіть коректний телефон відправника!");
        return;
    }

    if (phoneReciever.length() != 19) {
        ui->statusBar->showMessage("Введіть коректний телефон отримувача!");
        return;
    }

    ui->statusBar->clearMessage();

    int senderDepartmentIndex = ui->arrangeCitySenderComboBox->currentIndex();
    int recieverDepartmentIndex = ui->arrangeCityRecieverComboBox->currentIndex();
    QString type = ui->arrangeTypeComboBox->currentText();
    double length = ui->arrangeLengthLineEdit->text().toDouble();
    double width = ui->arrangeWidthLineEdit->text().toDouble();
    double height = ui->arrangeHeightLineEdit->text().toDouble();
    double weight = ui->arrangeWeightLineEdit->text().toDouble();
    int costInsurance = ui->arrangeCostInsuranceLineEdit->text().toInt();

    if (!Calculation::defineParamsParcelDependOfType(ui->statusBar, type, senderDepartmentIndex,
                                                     recieverDepartmentIndex, length, width,
                                                     height, weight, costInsurance)) {
        return;
    }

    bool packing = ui->arrangePackingCheckBox->isChecked();
    int amount = ui->arrangeAmountSpinBox->value();
    int cost = Calculation::calculateCost(senderDepartmentIndex, recieverDepartmentIndex,
                                          length, width, height, weight,
                                          costInsurance, packing, amount);
    QString senderCity = ui->arrangeCitySenderComboBox->currentText();
    QString receiverCity = ui->arrangeCitySenderComboBox->currentText();
    bool recieverPays = ui->arrangeRecieverPayCheckBox->isChecked();
    QString parcelPays = ui->arrangeParcelPayComboBox->currentText();

    int numberOrder =
            DBManager::instance().arrangeParcel(cost, senderCity, receiverCity, fullNameSender,
                                                fullNameReciever, phoneSender, phoneReciever,
                                                type, weight, length, width, height, costInsurance,
                                                recieverPays, packing, parcelPays);
    if (-1 == numberOrder) {
        ui->statusBar->showMessage("Помилка оформлення відправлення!");
        return;
    }

    ui->statusBar->showMessage("Відправлення №" + QString::number(numberOrder) +
                               " успішно сформовано!");
    ui->stackedWidget->setCurrentWidget(ui->mainMenuWidget);
    this->resetDefaultArrangeWidget();
}


void MainWindow::on_searchShowAllpushButton_clicked()
{
    ui->searchSearchLineEdit->clear();
    ordersModel_->setFilter("");
}


void MainWindow::on_searchSearchLineEdit_textChanged(const QString &arg1)
{
    ordersModel_->setFilter("");

    if (arg1 == "") {
        ui->statusBar->showMessage("Показані всі відправлення");
        return;
    }

    if (!ordersModel_->record(arg1.toInt() - 1).value("Key").isNull()) {
        ordersModel_->setFilter("Key = " + arg1);
        ui->searchShowAllTableView->selectRow(0);
        ui->statusBar->showMessage("Відправлення №" + arg1 + " знайдено!");
    } else {
        ordersModel_->setFilter("Key = 0");
        ui->statusBar->showMessage("Відправлення №" + arg1 + " не знайдено!");
    }
}


void MainWindow::on_statusInNumberLineEdit_textChanged(const QString &arg1)
{
    ui->statusStatusShowLabel->clear();
    ui->statusProgressBar->setValue(0);
    ui->statusStatusChangeComboBox->setCurrentIndex(-1);
    ui->statusStatusChangeComboBox->setDisabled(true);

    if (arg1 == "") {
        ordersModel_->setFilter("Key = 0");
        ui->statusBar->clearMessage();
        return;
    }

    ordersModel_->setFilter("");
    QString status = ordersModel_->record(arg1.toInt() - 1).value("StatusParcel").toString();

    if (status != "") {
        ordersModel_->setFilter("Key = " + arg1);
        ui->statusShowChangeTableView->selectRow(0);

        ui->statusStatusShowLabel->setText(status);
        ui->statusStatusChangeComboBox->setEnabled(true);
        ui->statusStatusChangeComboBox
                    ->setCurrentIndex(ui->statusStatusChangeComboBox->findText(status));

        int progress = this->defineProgress(ui->statusStatusChangeComboBox->currentText());
        ui->statusProgressBar->setValue(progress);
        ui->statusBar->showMessage("Відправлення №" + arg1 + " знайдено!");
    } else {
        ordersModel_->setFilter("Key = 0");
        ui->statusBar->showMessage("Відправлення №" + arg1 + " не знайдено!");
    }
}


void MainWindow::on_statusStatusChangeComboBox_currentTextChanged(const QString &arg1)
{
    ordersModel_->setData(ordersModel_->index(0, ordersModel_->fieldIndex("StatusParcel")), arg1);
    ordersModel_->submitAll();

    ui->statusStatusShowLabel->setText(arg1);
    int progress = this->defineProgress(arg1);
    ui->statusProgressBar->setValue(progress);

    QString numberOrder = ordersModel_->record(0).value("Key").toString();
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
    this->close();
}


void MainWindow::on_actionLogout_triggered()
{
    ui->passwordLineEdit->clear();
    ui->stackedWidget->setCurrentWidget(ui->loginWidget);
    ui->statusBar->clearMessage();

    ui->actionLogout->setDisabled(true);
    ui->actionShowDepartments->setDisabled(true);

    this->resetDefaultArrangeWidget();
    this->resetDefaultCalculateWidget();
    ui->statusInNumberLineEdit->clear();
    ui->searchSearchLineEdit->clear();
}


void MainWindow::on_loginPushButton_clicked()
{
    QSqlTableModel *usersModel = DBManager::instance().getUsersModel();
    int usersCount = usersModel->rowCount();

    for (int i = 0; i < usersCount; ++i) {
        QString login = usersModel->record(i).value("Login").toString();

        if (ui->loginLineEdit->text() == login) {
            QString password = usersModel->record(i).value("Password").toString();

            if (ui->passwordLineEdit->text() == password) {
                QString name = usersModel->record(i).value("FullName").toString();
                ui->mainGreetingLabel->setText("Обліковий запис: " + name);

                ui->stackedWidget->setCurrentWidget(ui->mainMenuWidget);
                ui->statusBar->showMessage("Успішний вхід!");

                ui->actionLogout->setEnabled(true);
                ui->actionShowDepartments->setEnabled(true);
                return;
            }
            ui->statusBar->showMessage("Невірний пароль!");
            ui->passwordLineEdit->clear();
            return;
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
    if (arg1 == "Посилка") {
        ui->calcWeightLineEdit->setEnabled(true);
        ui->calcWidthLineEdit->setEnabled(true);
        ui->calcLengthLineEdit->setEnabled(true);
        ui->calcHeightLineEdit->setEnabled(true);
    }
    else if (arg1 == "Документи") {
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
    if ("Посилка" == arg1) {
        ui->arrangeWeightLineEdit->setEnabled(true);
        ui->arrangeWidthLineEdit->setEnabled(true);
        ui->arrangeLengthLineEdit->setEnabled(true);
        ui->arrangeHeightLineEdit->setEnabled(true);
    }
    else if ("Документи" == arg1) {
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


int MainWindow::defineProgress(QString status)
{
    int progress = 0;
    if ("Комплектується" == status)
        progress = 20;
    else if ("В дорозі" == status)
        progress = 65;
    else if ("Доставлено" == status)
        progress = 100;
    return progress;
}


void MainWindow::on_calcClearToolButton_clicked()
{
    this->resetDefaultCalculateWidget();
    ui->statusBar->clearMessage();
}

