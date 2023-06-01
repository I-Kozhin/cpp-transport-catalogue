
/**
 * @file geo.cpp
 * @brief Contains the definition of the Coordinates structure and the ComputeDistance function.
 */

#define _USE_MATH_DEFINES
#include "geo.h"

#include <cmath>

namespace geo {

    static const double EarthRadius = 6371000.0; /**< Radius of the Earth in meters */


     /**
     * @brief Реализация вычисления расстояния между двумя координатами на сфере
     * @param from Начальные координаты
     * @param to Конечные координаты
     * @return Расстояние между координатами в метрах
     */
    double ComputeDistance(Coordinates from, Coordinates to) {
        using namespace std;
        if (from == to) {
            return 0;
        }
        static const double dr = M_PI / 180.;
        return acos(sin(from.lat * dr) * sin(to.lat * dr)
                    + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
            * EarthRadius;
    }

}  // namespace geo