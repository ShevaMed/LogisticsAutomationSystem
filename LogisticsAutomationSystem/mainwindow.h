#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void resetDefaultCalculateWidget();
    void resetDefaultArrangeWidget();
    int defineProgress(QString status);

private slots:
    void on_arrangeParcelButton_clicked();

    void on_calculateButton_clicked();

    void on_calcCalculatePushButton_clicked();

    void on_statusShowChangeButton_clicked();

    void on_searchShowAllButton_clicked();

    void on_arrangeArrangePushButton_clicked();

    void on_searchShowAllpushButton_clicked();

    void on_searchSearchLineEdit_textChanged(const QString &arg1);

    void on_statusInNumberLineEdit_textChanged(const QString &arg1);

    void on_statusStatusChangeComboBox_currentTextChanged(const QString &arg1);

    void on_calcMainMenuCommandLinkButton_clicked();

    void on_arrangeMainMenuCommandLinkButton_clicked();

    void on_statusMainMenuCommandLinkButton_clicked();

    void on_searchMainMenuCommandLinkButton_clicked();

    void on_actionExit_triggered();

    void on_actionLogout_triggered();

    void on_loginPushButton_clicked();

    void on_actionShowDepartments_triggered();

    void on_departmentsMainMenuCommandLinkButton_clicked();

    void on_calcTypeComboBox_currentTextChanged(const QString &arg1);

    void on_arrangeTypeComboBox_currentTextChanged(const QString &arg1);

    void on_calcClearToolButton_clicked();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *departmentsModel_;
    QSqlTableModel *ordersModel_;

};
#endif // MAINWINDOW_H
