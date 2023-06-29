#pragma once

/**
 * @file domain.h
 * @brief В этом файле размеotys структуры, которые являются частью предметной области (domain) вашего приложения и не зависят от транспортного справочника.
 */

#include "geo.h"

#include <string>
#include <vector>
#include "deque"

namespace domain {

	/**
     * @struct StopDistancesDescription
     *
     * @brief Struct representing a stop distances description with a stop name and distances to other stops.
     */
	struct StopDistancesDescription {
		std::string stop_name;
		std::vector<std::pair<std::string, int>> distances;
	};

	/**
     * @struct Stop
     *
     * @brief Struct representing a stop with a stop name and coordinates.
     */
	struct Stop {
		std::string stop_name;
		geo::Coordinates coordinates;

	};

	/**
     * @struct BusDescription
     *
     * @brief Struct representing a bus with a bus name, list of stops, and type.
     */
	struct BusDescription {
		std::string bus_name;
		std::vector<std::string> stops;
		std::string type;
	};

	/**
     * @struct AllBusInfoBusResponse
     *
     * @brief Struct representing the information of a bus response including the bus name, number of stops, number of unique stops, route length, and route curvature.
     */
	struct AllBusInfoBusResponse {
		std::string bus_name;
		int quant_stops;
		int quant_uniq_stops;
		double route_length = 0;
		double route_curvature = 0;
	};


	/**
     * @struct Bus
     *
     * @brief Struct representing a bus with a bus name, list of stops, and type.
     */
	struct Bus {
		std::string bus_name;
		std::deque<std::string_view> stops;
		std::string type;
	};

	/**
	 * @struct RouteSettings
	 * @brief Struct representing the settings for route calculation, including bus velocity and bus wait time.
	 */
	struct RouteSettings {
		double bus_velocity;
		double bus_wait_time;

	};
}  // namespace domain