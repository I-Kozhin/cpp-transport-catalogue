#pragma once

/** 
 *@file transport_catalogue.h
 *@brief This file contains the declaration of the TransportCatalogue class and related structs and functions.
 */

#include <string>
#include <algorithm>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <math.h>
#include "geo.h"
#include <functional> 

namespace transport_catalogue {
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
     * @struct StopInput
     *
     * @brief Struct representing a stop input with a stop name, coordinates, and distances to other stops.
     */
	struct StopInput {
        std::string stop_name;
        geo::Coordinates coordinates;
        std::vector<std::pair<std::string, int>> stop_dist;
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
		std::deque<std::string*> stops;
		std::string type;
	};

	namespace detail {
         /**
         * @struct PairOfStopPointerUsing_hash
         *
         * @brief Struct representing a hash function for a pair of stop pointers.
         */
		struct PairOfStopPointerUsing_hash {
            /**
             * @brief Calculates the hash value for a pair of stop pointers.
             *
             * @param p The pair of stop pointers.
             * @return The calculated hash value.
             */
			std::size_t operator()(const std::pair<Stop*, Stop*>& p) const {
				std::size_t hash1 = std::hash<Stop*>{}(p.first);
				std::size_t hash2 = std::hash<Stop*>{}(p.second);
				std::size_t combined_hash = hash1 + hash2;
				return combined_hash;
			}
		};

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
			std::size_t operator()(const std::pair<Stop*,   Stop*>& p) const {
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

            void AddBus(const BusDescription& bus);
            void AddStop(const StopInput& stop);
            const Bus* FindBus(std::string bus) const;

            const Stop* FindStop(const std::string* stop) const;
            AllBusInfoBusResponse GetAllBusInfo(std::string_view bus)  ;

            std::set<std::string> GetStopInfo(std::string_view s) const;

            void AddStopDistance(const StopInput& s);

            /**
             * @brief Retrieves the distance between two stops.
             *
             * @param s1 Pointer to the first stop.
             * @param s2 Pointer to the second stop.
             * @return The distance between the two stops, or 0 if not found.
             */
            int GetStopDistance( Stop* s1, Stop* s2)  const ;
            
        

        private:
            std::deque<Bus> buses_;
            std::deque<Stop> stops_;
            std::unordered_map<std::string, Stop*> stop_name_to_stop_;
            std::unordered_map<std::string_view, Bus*> bus_name_to_bus_;
            std::unordered_map<std::string_view, std::set<std::string>> stop_info_;
            std::unordered_map<std::pair<Stop*, Stop*>, int, detail::PairOfStopPointerUsingString> stops_distance_;
	};
}