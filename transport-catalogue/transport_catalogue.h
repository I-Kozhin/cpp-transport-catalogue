#pragma once

/** 
 *@file transport_catalogue.h
 *@brief This file contains the declaration of the TransportCatalogue class and related structs and functions.
 */

#include "domain.h"

#include <string>
#include "deque"
#include <unordered_set>
#include "geo.h"
#include <functional> 
#include "set"

namespace transport_catalogue {

	struct OutputRequest {
		int id;
		std::string type;
		std::string name;
	};

	namespace detail {

		/**
         * @struct PairOfStopPointerUsingString
         *
         * @brief Struct representing a hash function for a pair of stop pointers using string hashing.
         */
		struct PairOfStopPointerUsingString {

			/**
             * @brief Calculates the hash value for a pair of stop pointers using string hashing.
             *
             * @param p The pair of stop pointers.
             * @return The calculated hash value.
             */
			std::size_t operator()(const std::pair<const  domain::Stop*, const  domain::Stop*>& p) const {
				const std::size_t constantal = 31;
				std::size_t hash_value_one = 0;
				std::size_t hash_value_two = 0;
				std::size_t p_pow = 1;
				for (char c : p.first->stop_name) {
					hash_value_one = (hash_value_one + (c - 'a' + 1) * p_pow) % std::size_t(-1);
					p_pow = (p_pow * constantal) % std::size_t(-1);
				}

				for (char c : p.second->stop_name) {
					hash_value_one = (hash_value_two + (c - 'a' + 1) * p_pow) % std::size_t(-1);
					p_pow = (p_pow * constantal) % std::size_t(-1);
				}

				return hash_value_one + hash_value_two * constantal;
			}
		};
	}

	/**
     * @class TransportCatalogue
     *
     * @brief Class representing a transport catalogue with buses, stops, and related methods.
     */
	class TransportCatalogue {
		public:
			
			/**
         	 * @brief Adds a bus to the transport catalogue.
         	 *
         	 * @param bus The bus description.
         	 */
			void AddBus(const domain::BusDescription& bus);

			/**
         	 * @brief Adds a stop to the transport catalogue.
         	 *
         	 * @param stop The stop to add.
         	 */
			void AddStop(domain::Stop stop);

			/**
         	 * @brief Finds a bus by its name.
         	 *
         	 * @param bus The name of the bus.
         	 * @return A pointer to the found bus, or nullptr if not found.
         	 */
			const domain::Bus* FindBus(std::string_view bus) const;

			/**
         	 * @brief Finds a stop by its name.
         	 *
         	 * @param stop The name of the stop.
         	 * @return A pointer to the found stop, or nullptr if not found.
         	 */
			virtual const domain::Stop* FindStop(std::string_view stop) const;

			/**
         	 * @brief Retrieves the information of a bus, including the number of stops, number of unique stops, route length, and route curvature.
         	 *
         	 * @param bus The name of the bus.
         	 * @return The information of the bus.
         	 */
			domain::AllBusInfoBusResponse GetAllBusInfo(std::string_view bus) const;

			/**
         	 * @brief Retrieves the set of bus names that pass through a stop.
         	 *
         	 * @param s The name of the stop.
         	 * @return The set of bus names.
         	 */
			std::set<std::string> GetStopInfo(std::string_view s) const;

			/**
         	 * @brief Adds stop distances to the transport catalogue.
         	 *
         	 * @param distance The stop distances description.
         	 */
			void AddStopDistance(domain::StopDistancesDescription distance);

			/**
			 * @brief Retrieves the distance between two stops.
			 *
			 * @param s1 First stop.
			 * @param s2 Second stop.
			 * @return The distance between the two stops, or 0 if not found.
			 */
			int GetStopDistance(const domain::Stop& s1, const domain::Stop& s2)  const;

			/**
         	 * @brief Retrieves the list of buses in the transport catalogue.
         	 *
         	 * @return The list of buses.
         	 */
			const std::deque<domain::Bus>& GetBuses() const;

			/**
         	 * @brief Retrieves the list of stops in the transport catalogue.
         	 *
         	 * @return The list of stops.
         	 */
			const std::deque<domain::Stop>& GetStops() const;

		private:
			std::deque<domain::Bus> buses_;		/**< The list of buses */
			std::deque<domain::Stop> stops_;	/**< The list of stops */
        	std::unordered_map<std::string_view, domain::Stop*> stop_name_to_stop_; /**< The map of stop names to stop pointers */
        	std::unordered_map<std::string_view, domain::Bus*> bus_name_to_bus_; 	/**< The map of bus names to bus pointers */
        	std::unordered_map<std::string_view, std::set<std::string>> stop_info_; /**< The map of stop names to set of bus names */
			std::unordered_map<std::pair<const domain::Stop*, const domain::Stop*>, int, detail::PairOfStopPointerUsingString> stops_distance_; /**< The map of pairs of stop pointers to distance */
	};
}  // namespace transport_catalogue