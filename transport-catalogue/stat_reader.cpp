/**
 * @file stat_reader.cpp
 * @brief Implementation of the StatReader class
 */

#include <iostream>
#include "stat_reader.h"
#include "transport_catalogue.h"

using namespace std;

namespace transport_catalogue {

	/**
	 * @brief Получает количество выходных запросов из входного потока
	 * 
	 * Этот метод считывает строку из входного потока и пытается преобразовать ее в целое число с помощью std::stoi.
	 * Если строка не является пустой, выполняется преобразование. Если во время преобразования происходит ошибка,
	 * количество выходных запросов устанавливается в 0. Если строка пуста, количество выходных запросов также устанавливается в 0.
	 * 
	 * @return Количество выходных запросов
	 */
	int StatReader::GetNumOutQueries() {
		string line;
		getline(input_stream_, line);
		if (!line.empty()) {
			try {
				num_update_queries_ = std::stoi(line);
			} 
            catch (const std::invalid_argument&) {
				num_update_queries_ = 0;
			}
		} 
        else {
			num_update_queries_ = 0;
		}
		return num_update_queries_;
	}

	/**
	 * @brief Заполняет вектор запросов данными
	 * 
	 * Этот метод считывает строки из входного потока и заполняет вектор запросов объектами типа OutputRequest.
	 * Каждая строка разделяется на две части: имя и запрос. Эти части сохраняются в объекте OutputRequest.
	 */
	void StatReader::FillRequests() {
		for (int i = 0; i < num_update_queries_; ++i) {
			OutputRequest r;
			string line;
			getline(input_stream_, line);
			auto space_colon = line.find(" ");
			r.name = line.substr(0, space_colon);
			r.request = line.substr(space_colon + 1);
			requests_.push_back(r);
		}
	}

	/**
	 * @brief Выводит результаты запросов в стандартный вывод
	 * 
	 * Этот метод обрабатывает каждый запрос из вектора запросов и выводит результаты в стандартный вывод.
	 * Для запросов типа "Bus" он вызывает метод GetAllBusInfo объекта TransportCatalogue и выводит соответствующую информацию.
	 * Для запросов типа "Stop" он вызывает методы FindStop и GetStopInfo объекта TransportCatalogue и выводит соответствующую информацию.
	 * 
	 * @param tc Объект TransportCatalogue, используемый для обработки запросов
	 */
	void StatReader::Output(ostream& os, TransportCatalogue& tc) {
		for (const auto& element : requests_) {
			if (element.name == "Bus"s) {
				AllBusInfoBusResponse r = tc.GetAllBusInfo(element.request);
				if (r.quant_stops == 0) {
					os << "Bus " << element.request << ": "s << "not found" << endl;
				}
				else {
					os << "Bus " << r.bus_name << ": "s << r.quant_stops << " stops on route, "s << r.quant_uniq_stops << " unique stops, "s << r.route_length << " route length, "s << r.route_curvature << " curvature"s << endl;
				}
			}

			if (element.name == "Stop"s) {
				const Stop* myStop = tc.FindStop(&element.request);
				if (myStop == nullptr) {
					os << "Stop " << element.request << ": not found" << endl;
				}
				else {
					set<string> r = tc.GetStopInfo(element.request);
					if (r.empty()) {
						os << "Stop " << element.request << ": no buses" << endl;
					}
					else {
						os << "Stop " << element.request << ": "s << "buses ";
						for (const auto& el : r) {
							os << el << " ";
						}
						os << endl;
					}
				}
			}
		}
	}

}
