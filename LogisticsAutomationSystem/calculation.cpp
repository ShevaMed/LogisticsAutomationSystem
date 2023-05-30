#include "calculation.h"
#include "dbmanager.h"

#include <cmath>
#include <QSqlRecord>

Calculation::Calculation()
{

}

double Calculation::calculateTheDistanceOnTheEarth(double latitudeA, double longitudeA, double latitudeB, double longitudeB)
{
    // convert coordinates to radians
    double latA = latitudeA * M_PI / 180;
    double latB = latitudeB * M_PI / 180;
    double longA = longitudeA * M_PI / 180;
    double longB = longitudeB * M_PI / 180;

    // cosines and sines of latitudes and longitude differences
    double cos_latA = cos(latA);
    double cos_latB = cos(latB);
    double sin_latA = sin(latA);
    double sin_latB = sin(latB);
    double delta = longB - longA;
    double cos_delta = cos(delta);
    double sin_delta = sin(delta);

    // calculate the length of the great circle
    double x = sin_latA * sin_latB + cos_latA * cos_latB * cos_delta;
    double y = sqrt((cos_latB * sin_delta) * (cos_latB * sin_delta) +
                    (cos_latA * sin_latB - sin_latA * cos_latB * cos_delta) *
                    (cos_latA * sin_latB - sin_latA * cos_latB * cos_delta));

    return atan2(y, x) * 6371;
}

double Calculation::calculateCost(int senderDepartmentIndex, int recieverDepartmentIndex,
                                  double length, double width, double height, double weight,
                                  int costInsurance, bool packing, int amount)
{
    QSqlTableModel *departmentsModel = DBManager::instance().getDepartmentsModel();
    double senderLatitude = departmentsModel->record(senderDepartmentIndex)
            .value("GeographicalLatitude").toDouble();
    double senderLongitude = departmentsModel->record(senderDepartmentIndex)
            .value("GeographicalLongitude").toDouble();
    double recieverLatitude = departmentsModel->record(recieverDepartmentIndex)
            .value("GeographicalLatitude").toDouble();
    double recieverLongitude = departmentsModel->record(recieverDepartmentIndex)
            .value("GeographicalLongitude").toDouble();

    double distance = Calculation::calculateTheDistanceOnTheEarth(senderLatitude, senderLongitude,
                                                     recieverLatitude, recieverLongitude);
    double volume = (length * width * height) / 1000;
    double profile = (volume > weight) ? volume : weight;

    double cost = 50 + distance * 0.1 + profile * 5 + costInsurance * 0.005;

    if (packing) {
        cost += volume * 8;
    }

    cost *= amount;
    return cost;
}

bool Calculation::defineParamsParcelDependOfType(QStatusBar *statusBar, QString type,
                                                 int senderDepartmentIndex,
                                                 int recieverDepartmentIndex,
                                                 double& length, double& width,
                                                 double& height, double& weight,
                                                 int costInsurance)
{
    if (-1 == senderDepartmentIndex || -1 == recieverDepartmentIndex) {
        statusBar->showMessage("Введіть маршрут!");
        return false;
    }

    if (senderDepartmentIndex == recieverDepartmentIndex) {
        statusBar->showMessage("Некоректний маршрут! Міста повинні бути різними");
        return false;
    }

    if (!costInsurance) {
        statusBar->showMessage("Введіть коректну ціну страхування!");
        return false;
    }

    if (type == "Посилка") {
        if (weight == 0.0) {
            statusBar->showMessage("Введіть коректну вагу відправлення!");
            return false;
        }

        if (length == 0.0 || width == 0.0 || height == 0.0) {
            statusBar->showMessage("Введіть коректно всі розміри відправлення!");
            return false;
        }
    }
    else if (type == "Документи") {
        length = 30;
        width = 23;
        height = 2;
        weight = 0.2;
    }

    return true;
}
