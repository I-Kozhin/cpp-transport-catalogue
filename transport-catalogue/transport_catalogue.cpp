/**
 * @file transport_catalogue.cpp
 * @brief Implementation of the TransportCatalogue class.
 */


#include "input_reader.h"
#include "geo.h"
#include <string_view>
#include "transport_catalogue.h"



using namespace std;
namespace transport_catalogue {

    /**
     * @brief Добавляет автобус в транспортный каталог.
     * @param b Структура BusDescription с информацией об автобусе.
     */
	void TransportCatalogue::AddBus(const BusDescription& bus_desc) {
		Bus bptr;
		deque <std::string*> stops_ptr;
		for (const auto& stop : bus_desc.stops) {   
			auto it = stop_name_to_stop_.find(stop);
			if (it != stop_name_to_stop_.end()) {
				stops_ptr.push_back(&it->second->stop_name);
		
			}
		}
		bptr.bus_name = bus_desc.bus_name;
		bptr.type = bus_desc.type;
		bptr.stops = stops_ptr;
		buses_.push_back(bptr);
		Bus* bptr_bus = &buses_.back();
		bus_name_to_bus_.emplace(bus_desc.bus_name, bptr_bus);
		for (auto el : stops_ptr) {
			stop_info_[*el].insert(bus_desc.bus_name);
		}
	}

    /**
     * @brief Добавляет остановку в транспортный каталог.
     * @param stop Структура Stop_Input с информацией об остановке.
     */
	void TransportCatalogue::AddStop(const StopInput& stop) {
		Stop stop_input;
		stop_input.stop_name = stop.stop_name;
		stop_input.coordinates = stop.coordinates;
		stops_.push_back(stop_input);
		Stop* ptr_stop = &stops_.back();
		stop_name_to_stop_.emplace(stop.stop_name, ptr_stop);
		}
	
    /**
     * @brief Находит автобус по его названию.
     * @param bus Название автобуса.
     * @return Указатель на структуру Bus с информацией о найденном автобусе или nullptr, если автобус не найден.
     */
	const Bus* TransportCatalogue::FindBus(string bus) const  {
		const Bus* res= nullptr;
		if (bus_name_to_bus_.count(bus)) { 
            return static_cast<const Bus*>(bus_name_to_bus_.at(bus)); 
        }
		else { 
            return res; 
        }
	}

    /**
     * @brief Находит остановку по ее названию.
     * @param stop Указатель на название остановки.
     * @return Указатель на структуру Stop с информацией о найденной остановке или nullptr, если остановка не найдена.
     */
	const Stop* TransportCatalogue::FindStop(const std::string* stop) const {
		const Stop* res = nullptr;
		
		if (stop_name_to_stop_.count(*stop)) { 
            return static_cast<const Stop*>(stop_name_to_stop_.at(*stop)); 
        }
		else { 
            return res; 
        }
	}

    /**
     * @brief Получает информацию о маршруте автобуса.
     * @param bus Название автобуса.
     * @return Структура AllBusInfoBusResponse с информацией о маршруте автобуса.
     */
	AllBusInfoBusResponse TransportCatalogue::GetAllBusInfo(string_view bus) {
		AllBusInfoBusResponse bus_info;
		const Bus* found_bus = FindBus(string(bus));
		if (found_bus) {
			deque<string*> stops_v = found_bus->stops;
			double coord_length = 0;
			int real_length = 0;
			if (stops_v.size() != 0) {
				bus_info.bus_name = bus;

				if (found_bus->type == "c"s) {
					bus_info.quant_stops = stops_v.size();
					unordered_set<string*> unique_stops(stops_v.begin(), stops_v.end());
					bus_info.quant_uniq_stops = unique_stops.size();
					for (int i = 0; i < stops_v.size() - 1; i++) {
						const Stop* one = FindStop( stops_v[i]   );
						const Stop* two = FindStop(stops_v[i + 1]);
						coord_length += geo::ComputeDistance(one->coordinates, two->coordinates);
						real_length += GetStopDistance(const_cast<Stop*>(one), const_cast<Stop*>(two));
					}
				}
				else {
					bus_info.quant_stops = stops_v.size() * 2 - 1;
					unordered_set<string*> unique_stops(stops_v.begin(), stops_v.end());
					bus_info.quant_uniq_stops = unique_stops.size();
					for (int i = 0; i < stops_v.size() - 1; i++) {
						const Stop* one = FindStop(stops_v[i]);
						const Stop* two = FindStop(stops_v[i + 1]);
						coord_length += geo::ComputeDistance(one->coordinates, two->coordinates);
						real_length += GetStopDistance(const_cast<Stop*>(one), const_cast<Stop*>(two));
					}
					for (auto it = stops_v.rbegin(); it != stops_v.rend(); ++it) {
						if (it != stops_v.rbegin()) {

							const Stop* two = FindStop(*it);
							const Stop* one = FindStop(*(it - 1));

							string st_one_name = one->stop_name;
							string st_two_name = two->stop_name;

							int new_length = GetStopDistance(const_cast<Stop*>(one), const_cast<Stop*>(two));

							real_length += new_length;
						}
					}
					coord_length += coord_length;
				}

				bus_info.route_length = real_length;
				bus_info.route_curvature = real_length / coord_length;
			}
		}
		else {
			bus_info.bus_name = bus; bus_info.quant_stops = 0;
		}
		return bus_info;
	}

    /**
     * @brief Получает информацию об автобусах, проходящих через указанную остановку.
     * @param s Название остановки.
     * @return Множество строк с названиями автобусов, проходящих через указанную остановку.
     */
	set<string> TransportCatalogue::GetStopInfo(std::string_view stop_name) const {
		set<string> res;
		if (stop_info_.count(stop_name)) {
			return stop_info_.at(stop_name);
		}
		else { return res; }

	}

    /**
     * @brief Добавляет расстояния между остановками в транспортный каталог.
     * @param s Структура Stop_Input с информацией об остановке и расстояниях.
     */
	void TransportCatalogue::AddStopDistance(const StopInput& s) {
		if (s.stop_dist.size() != 0) {
			Stop* main_stop_ptr = stop_name_to_stop_[s.stop_name];
			vector<pair<string, int>> stop_dist_main = s.stop_dist;
			for (auto el : stop_dist_main) {
				Stop* another_stop_ptr = stop_name_to_stop_[el.first];
				int distance = el.second;
				stops_distance_.emplace(make_pair(main_stop_ptr, another_stop_ptr), distance);
			}
		}
	}

    /**
     * @brief Получает расстояние между двумя остановками.
     * @param s1 Указатель на первую остановку.
     * @param s2 Указатель на вторую остановку.
     * @return Расстояние между остановками или 0, если оно не найдено.
     */
	int TransportCatalogue::GetStopDistance(Stop* s1, Stop* s2) const {
		auto it = stops_distance_.find(make_pair(s1, s2));
		auto it2 = stops_distance_.find(make_pair(s2, s1));

		if (it != stops_distance_.end()) {
			return stops_distance_.at(make_pair(s1, s2));
		}
		else if (it2 != stops_distance_.end()) {
			return stops_distance_.at(make_pair(s2, s1));
		}
		else { return 0; }
	}
}   // namespace transport_catalogue