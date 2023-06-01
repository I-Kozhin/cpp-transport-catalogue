#pragma once

/**
 * @file geo.h
 * @brief Contains the definition of the Coordinates structure and the ComputeDistance function.
 */

namespace geo {

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
     * @brief Объявление вычисления расстояния между двумя координатами на сфере
     * @param from Начальные координаты
     * @param to Конечные координаты
     * @return Расстояние между координатами в метрах
     */
    double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo