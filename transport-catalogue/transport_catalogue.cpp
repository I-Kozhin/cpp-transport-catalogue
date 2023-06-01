/**
 * @file transport_catalogue.cpp
 * @brief Implementation of the TransportCatalogue class.
 */

#include "json_reader.h"
#include "geo.h"
#include "transport_catalogue.h"


using namespace std;
using namespace domain;
namespace transport_catalogue {

	/**
     * @brief Добавляет автобус в транспортный каталог.
	 * 
     * @param bus_desc Структура BusDescription с информацией об автобусе.
     */
	void TransportCatalogue::AddBus(const BusDescription& bus_desc) {
		Bus bptr;
		deque <std::string_view> stops_ptr;
		for (const auto& stop : bus_desc.stops) {
			auto it = stop_name_to_stop_.find(stop);
			if (it != stop_name_to_stop_.end()) {
				stops_ptr.push_back(it->second->stop_name);

			}
		}
		bptr.bus_name = bus_desc.bus_name;
		bptr.type = bus_desc.type;
		bptr.stops = stops_ptr;
		buses_.push_back(bptr);
		Bus* bptr_bus = &buses_.back();
		bus_name_to_bus_.emplace(bus_desc.bus_name, bptr_bus);
		for (auto el : stops_ptr) {
			stop_info_[el].insert(bus_desc.bus_name);
		}
	}

	/**
     * @brief Adds a stop to the transport catalogue.
     *
     * @param stop The Stop structure with information about the stop.
     */
	void TransportCatalogue::AddStop(Stop stop) {
		stops_.push_back(move(stop));
		Stop* ptr_stop = &stops_.back();
		stop_name_to_stop_.emplace(string_view(ptr_stop->stop_name), ptr_stop);
	}

	/**
     * @brief Находит автобус по его названию.
     * @param bus Название автобуса.
     * @return Указатель на структуру Bus с информацией о найденном автобусе или nullptr, если автобус не найден.
     */
	const Bus* TransportCatalogue::FindBus(string_view bus) const {
		const Bus* res = nullptr;
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
	const Stop* TransportCatalogue::FindStop(std::string_view stop) const {
		const Stop* res = nullptr;

		if (stop_name_to_stop_.count(stop)) {
			 return static_cast<const Stop*>(stop_name_to_stop_.at(stop)); 
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
	AllBusInfoBusResponse TransportCatalogue::GetAllBusInfo(string_view bus)  const {
		AllBusInfoBusResponse bus_info;
		const Bus* found_bus = FindBus(string(bus));
		if (found_bus) {
			deque<string_view> stops_v = found_bus->stops;
			double coord_length = 0;
			int real_length = 0;
			if (stops_v.size() != 0) {
				bus_info.bus_name = bus;

				if (found_bus->type == "true"s) {
					bus_info.quant_stops = stops_v.size();
					unordered_set<string_view> unique_stops(stops_v.begin(), stops_v.end());
					bus_info.quant_uniq_stops = unique_stops.size();
					for (int i = 0; i < stops_v.size() - 1; i++) {
						const Stop* one = FindStop(stops_v[i]);
						const Stop* two = FindStop(stops_v[i + 1]);
						coord_length += geo::ComputeDistance(one->coordinates, two->coordinates);
						real_length += GetStopDistance(*one, *two);
					}
				}
				else {
					bus_info.quant_stops = stops_v.size() * 2 - 1;
					unordered_set<string_view> unique_stops(stops_v.begin(), stops_v.end());
					bus_info.quant_uniq_stops = unique_stops.size();
					for (int i = 0; i < stops_v.size() - 1; i++) {
						const Stop* one = FindStop(stops_v[i]);
						const Stop* two = FindStop(stops_v[i + 1]);
						coord_length += geo::ComputeDistance(one->coordinates, two->coordinates);
						real_length += GetStopDistance(*one, *two);
					}
					for (auto it = stops_v.rbegin(); it != stops_v.rend(); ++it) {
						if (it != stops_v.rbegin()) {

							const Stop* two = FindStop(*it);
							const Stop* one = FindStop(*(it - 1));

							string st_one_name = one->stop_name;
							string st_two_name = two->stop_name;

							int new_length = GetStopDistance(*one, *two);

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
	set<string> TransportCatalogue::GetStopInfo(std::string_view s) const {
		set<string> res;
		if (stop_info_.count(s)) {
			return stop_info_.at(s);
		}
		else { return res; }

	}

	/**
     * @brief Adds distances between stops to the transport catalogue.
     *
     * @param distance The StopDistancesDescription structure with stop information and distances.
     */
	void TransportCatalogue::AddStopDistance(StopDistancesDescription distance) {
		if (distance.distances.size() != 0) {
			Stop* main_stop_ptr = stop_name_to_stop_[string_view(distance.stop_name)];
			vector<pair<string, int>> stop_dist_main = distance.distances;
			for (auto el : stop_dist_main) {
				Stop* another_stop_ptr = stop_name_to_stop_[el.first];
				int distance = el.second;
				stops_distance_.emplace(make_pair(main_stop_ptr, another_stop_ptr), distance);
			}
		}
	}

	/**
     * @brief Получает расстояние между двумя остановками.
     * @param s1 Первая остановка.
     * @param s2 Вторая остановка.
     * @return Расстояние между остановками или 0, если оно не найдено.
     */
	int TransportCatalogue::GetStopDistance(const Stop& s1, const Stop& s2) const {
		const Stop* s1_ptr = &s1;
		const Stop* s2_ptr = &s2;
		auto it = stops_distance_.find(make_pair(s1_ptr, s2_ptr));
		auto it2 = stops_distance_.find(make_pair(s2_ptr, s1_ptr));

		if (it != stops_distance_.end()) {
			return stops_distance_.at(make_pair(s1_ptr, s2_ptr));
		}
		else if (it2 != stops_distance_.end()) {
			return stops_distance_.at(make_pair(s2_ptr, s1_ptr));
		}
		else { return 0; }
	}

	/**
     * @brief Returns a constant reference to the deque of buses.
     *
     * @return A constant reference to the deque of buses.
     */
	const std::deque<Bus>& TransportCatalogue::GetBuses() const { 
		return buses_; 
	}

	/**
     * @brief Returns a constant reference to the deque of stops.
     *
     * @return A constant reference to the deque of stops.
     */
	const std::deque<Stop>& TransportCatalogue::GetStops() const { 
		return stops_; 
	}

}   // namespace transport_catalogue