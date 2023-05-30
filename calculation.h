#ifndef CALCULATION_H
#define CALCULATION_H

#include <QStatusBar>

class Calculation
{
public:
    Calculation();
    static double calculateTheDistanceOnTheEarth(double latitudeA, double longitudeA,
                                                 double latitudeB, double longitudeB);

    static double calculateCost(int senderDepartmentIndex, int recieverDepartmentIndex,
                                double length, double width, double height, double weight,
                                int costInsurance, bool packing, int amount);

    static bool defineParamsParcelDependOfType(QStatusBar *statusBar, QString type,
                                               int senderDepartmentIndex,
                                               int recieverDepartmentIndex,
                                               double& length, double& width,
                                               double& height, double& weight,
                                               int costInsurance);
};

#endif // CALCULATION_H
