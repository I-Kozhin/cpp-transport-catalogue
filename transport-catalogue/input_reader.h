#pragma once

/**
 * @file input_reader.h
 * @brief Declaration of the InputReader class.
 */

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <deque>
#include <fstream>
#include <sstream>

#include "transport_catalogue.h"
#include "geo.h"

namespace transport_catalogue {

    /**
     * @brief Класс InputReader представляет собой считыватель входных данных.
     */
    class InputReader {
        public:
            /**
             * @brief Конструктор класса InputReader.
             * @param is Входной поток данных.
             */
            explicit InputReader(std::istream& is);

            /**
             * @brief Получает количество запросов на обновление.
             * @return Количество запросов на обновление.
             */
            int GetNumUpdateQueries();

            /**
             * @brief Заполняет список запросов на обновление.
             */
            void FillRequests();

            /**
             * @brief Обновляет остановки в транспортном каталоге.
             * @param tc Транспортный каталог.
             */
            void UpdStop(TransportCatalogue& tc);

            /**
             * @brief Обновляет информацию о маршрутах автобусов в транспортном каталоге.
             * @param tc Транспортный каталог.
             */
            void UpdBus(TransportCatalogue& tc);

            /**
             * @brief Обновляет расстояния между остановками в транспортном каталоге.
             * @param tc Транспортный каталог.
             */
            void UpdStopDist(TransportCatalogue& tc);

        private:
            std::istream& input_stream_;
            std::deque<BusDescription> update_requests_bus_;
            std::deque<StopInput> update_requests_stop_;
            int num_update_queries_;
    };

}  // namespace transport_catalogue
