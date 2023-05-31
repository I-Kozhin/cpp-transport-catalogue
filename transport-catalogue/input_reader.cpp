/**
 *@file input_reader.cpp
 *@brief Implementation of the InputReader class.
 */

#include "input_reader.h"
#include "transport_catalogue.h"

using namespace std;

namespace transport_catalogue {

    /**
     * @brief Разделяет строку на пары значений с расстоянием.
     * @param distance_section Секция строки с расстояниями.
     * @return Вектор пар значений, где первое значение - название остановки, второе - расстояние.
     */
    vector<std::pair<string, int>> SplitStringByForDist(const string& distance_section) {
        vector<std::pair<string, int>> result;
        vector<string> str_set;
        std::stringstream string_stream(distance_section);
        std::string substring;
        while (std::getline(string_stream, substring, ',')) {
            str_set.push_back(substring);
        }
        
        for (string str : str_set) {
            auto first_space_colon = str.find_first_not_of(" ");
            auto last_space_colon = str.find_last_not_of(" ");
            string distance_indicator = "m to ";
            auto distance_pos = str.find(distance_indicator);
            str = str.substr(first_space_colon, last_space_colon + 1);
            string distance_string = str.substr(0, distance_pos);
            int dist = std::stoi(str.substr(0, distance_pos));
            size_t second_space_pos = str.find(" ", str.find(" ") + 1);
            string stop_name = str.substr(second_space_pos + 1);//+1
            result.push_back(pair(stop_name, dist));
        }
        return result;
    }

    /**
     * @brief Разделяет строку по символу '-'.
     * @param str Исходная строка.
     * @return Вектор разделенных строк.
     */
    vector<string> SplitStringBySignPlain(const string& str) {
        vector<string> token_list;
        stringstream string_stream(str);
        string token;
        while (getline(string_stream, token, '-')) {
            auto first_space_colon = token.find_first_not_of(" ");
            auto last_space_colon = token.find_last_not_of(" ");
            token = token.substr(first_space_colon, last_space_colon);
            token_list.push_back(token);
        }
        return token_list;
    }

    /**
     * @brief Разделяет строку по символу '>'.
     * @param str Исходная строка.
     * @return Вектор разделенных строк.
     */
    vector<string> SplitStringBySignCircle(const string& str) {
        vector<string> token_list;
        stringstream string_stream(str);
        string token;
        while (getline(string_stream, token, '>')) {
            auto first_space_colon = token.find_first_not_of(" ");
            auto last_space_colon = token.find_last_not_of(" ");
            token = token.substr(first_space_colon, last_space_colon);
            token_list.push_back(token);
        }

        return token_list;
    }

    /**
     * @brief Разделяет строку по символу ',' и возвращает пару значений.
     * @param str Исходная строка.
     * @return Пара значений, где первое значение - широта, второе - долгота.
     */
    pair<double, double> SplitStringByComma(const string& str) {
        pair<double, double> coordinates;
        stringstream string_stream(str);
        string token;

        getline(string_stream, token, ',');
        coordinates.first = stod(token);

        getline(string_stream, token, ',');
        coordinates.second = stod(token);

        return coordinates;
    }

    /**
     * @brief Конструктор класса InputReader.
     * @param is Входной поток данных.
     */
    InputReader::InputReader(istream& is) : input_stream_(is) {
    }

    /**
     * @brief Получает количество запросов на обновление.
     * @return Количество запросов на обновление.
     */
    int InputReader::GetNumUpdateQueries() {
        string line;
        getline(input_stream_, line);
        num_update_queries_ = std::stoi(line);

        return num_update_queries_;
    }

    /**
     * @brief Заполняет список запросов на обновление.
     */
    void InputReader::FillRequests() {
        for (int i = 0; i < num_update_queries_; ++i) {
            string line;
            getline(input_stream_, line);
            auto pos_colon = line.find(":");
            string request_section = line.substr(0, pos_colon);
            string list_section = line.substr(pos_colon + 1);
            auto space_colon = request_section.find(" ");
            string request_name = request_section.substr(0, space_colon);
            string distance_indicator = "m to ";
            auto is_distance = list_section.find(distance_indicator);

            if (request_name == "Bus"s) {
                BusDescription q;
                auto circle = list_section.find(">");
                if (circle != std::string::npos) {
                    vector<string> bus_stops = SplitStringBySignCircle(list_section);
                    q.type = "c"s;
                    q.bus_name = request_section.substr(space_colon + 1, pos_colon);
                    q.stops = bus_stops;
                    update_requests_bus_.push_back(q);
                }
                else {
                    vector<string> bus_stops = SplitStringBySignPlain(list_section);
                    q.type = "p"s;
                    q.bus_name = request_section.substr(space_colon + 1, pos_colon);
                    q.stops = bus_stops;
                    update_requests_bus_.push_back(q);
                }
            }
            else if (request_name == "Stop"s && is_distance == std::string::npos) {
                StopInput q;
                q.stop_name = request_section.substr(space_colon + 1, pos_colon);
                pair<double, double> coordinates = SplitStringByComma(list_section);
                q.coordinates.lat = coordinates.first;
                q.coordinates.lng = coordinates.second;
                update_requests_stop_.push_back(q);
            }
            else if (request_name == "Stop"s && is_distance != std::string::npos) {
                StopInput q;
                q.stop_name = request_section.substr(space_colon + 1, pos_colon);

                size_t pos_sec_comma = list_section.find(",", list_section.find(",") + 1);
                if (pos_sec_comma != std::string::npos) {
                    vector<std::pair<string, int>> stop_dist_updt;
                    string cordinate_section = list_section.substr(1, pos_sec_comma);
                    string distance_section = list_section.substr(pos_sec_comma + 2);

                    pair<double, double> coordinates = SplitStringByComma(cordinate_section);
                    stop_dist_updt = SplitStringByForDist(distance_section);
                    q.coordinates.lat = coordinates.first;
                    q.coordinates.lng = coordinates.second;
                    q.stop_dist = stop_dist_updt;
                }
                update_requests_stop_.push_back(q);
            }
            else { 
                continue; 
            }
        }
    }

    /**
     * @brief Обновляет остановки в транспортном каталоге.
     * @param tc Транспортный каталог.
     */
    void InputReader::UpdStop(TransportCatalogue& tc) {
        for (int i = 0; i < update_requests_stop_.size(); ++i) {
            tc.AddStop(update_requests_stop_[i]);
        }
    }

    /**
     * @brief Обновляет расстояния между остановками в транспортном каталоге.
     * @param tc Транспортный каталог.
     */
    void InputReader::UpdStopDist(TransportCatalogue& tc) {
        for (int i = 0; i < update_requests_stop_.size(); ++i) {
            tc.AddStopDistance(update_requests_stop_[i]);
        }
    }

    /**
     * @brief Обновляет информацию о маршрутах автобусов в транспортном каталоге.
     * @param tc Транспортный каталог.
     */
    void InputReader::UpdBus(TransportCatalogue& tc) {
        for (int i = 0; i < update_requests_bus_.size(); ++i) {
            tc.AddBus(update_requests_bus_[i]);
        }
    }
}
