#pragma once

/**
 * @file geo.h
 * @brief Contains the definition of the Coordinates structure and the ComputeDistance function.
 */

#include <cmath>

namespace transport_catalogue {
    namespace geo {

        static const double EarthRadius = 6371000.0; /**< Radius of the Earth in meters */

        /**
         * @brief Структура для представления координат (широта и долгота)
         */
        struct Coordinates {
            double lat; /**< Широта */
            double lng; /**< Долгота */

            /**
             * @brief Перегруженный оператор равенства для сравнения координат
             * @param other Другие координаты для сравнения
             * @return true, если координаты равны, иначе false
             */
            bool operator==(const Coordinates& other) const {
                return lat == other.lat && lng == other.lng;
            }

            /**
             * @brief Перегруженный оператор неравенства для сравнения координат
             * @param other Другие координаты для сравнения
             * @return true, если координаты не равны, иначе false
             */
            bool operator!=(const Coordinates& other) const {
                return !(*this == other);
            }
        };

        /**
         * @brief Вычисление расстояния между двумя координатами на сфере
         * @param from Начальные координаты
         * @param to Конечные координаты
         * @return Расстояние между координатами в метрах
         */
        inline double ComputeDistance(Coordinates from, Coordinates to) {
            using namespace std;
            if (from == to) {
                return 0;
            }
            static const double dr = 3.1415926535 / 180.;
            return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
                * EarthRadius;
        }

    }  // namespace geo
}  // namespace transport_catalogue
