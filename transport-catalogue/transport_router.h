#pragma once
/**
 * @file transport_router.h
 * @brief This file contains the declaration of the ActivityProcessor class and related structs.
*/
#include "router.h"
#include "transport_catalogue.h"

#include <variant>
#include <memory>

namespace graph {
    /**
     * @struct BusActivity
     * @brief Struct representing a bus activity in the route.
     */
    struct BusActivity {
        std::string bus_name; /**< The name of the bus */
        double time; /**< The time spent on the bus activity */
        int span_count; /**< The number of stops spanned by the bus activity */
    };

    /**
     * @struct WaitingActivity
     * @brief Struct representing a waiting activity at a stop.
     */
    struct WaitingActivity {
        std::string stop_name_from; /**< The name of the stop where the waiting activity occurs */
        double time; /**< The time spent on the waiting activity */
    };

    /**
     * @struct DestinatioInfo
     * @brief Struct representing the destination information, including the route and total time.
     */
    struct DestinatioInfo {
        std::vector<std::variant<BusActivity, WaitingActivity>> route; /**< The route with bus activities and waiting activities */
        double all_time = 0.0; /**< The total time of the destination */
    };

    /**
     * @class ActivityProcessor
     * @brief Class responsible for processing activities and finding routes between stops.
     */
	class ActivityProcessor {

        public:
            /**
             * @brief Constructor for the ActivityProcessor class.
             * @param tc The transport catalogue containing bus and stop information.
             */
            ActivityProcessor(transport_catalogue::TransportCatalogue& tc);

            /**
             * @brief Adds knots (vertices) to the graph based on the bus routes.
             */
            void AddKnots();

            /**
             * @brief Finds the route and buses between two stops.
             * @param stop_name_from The name of the starting stop.
             * @param stop_name_to The name of the destination stop.
             * @return An optional DestinatioInfo struct containing the route and total time, or std::nullopt if the route is not found.
             */
            std::optional<DestinatioInfo> GetRouteAndBuses(std::string_view stop_name_from, std::string_view stop_name_to);

        private:
            transport_catalogue::TransportCatalogue& tc; /**< The transport catalogue */
            DirectedWeightedGraph<double> graph_; /**< The directed weighted graph representing the activities and routes */
            std::unordered_map<std::string_view, size_t> stop_to_vertex_; /**< The map of stop names to vertex indices in the graph */
            std::unique_ptr<graph::Router<double>> router_; /**< The router for finding routes in the graph */



            /**
             * @brief Retrieves the value associated with a key in the stop_to_vertex_ map.
             * @param key The stop name.
             * @return An optional size_t value representing the vertex index, or std::nullopt if the key is not found.
             */
            std::optional<size_t> GetValueByKey(std::string_view key);

            /**
             * @brief Checks if a stop exists in the stop_to_vertex_ map.
             * @param key The stop name.
             * @return True if the stop exists, false otherwise.
             */
            bool ChekExistValue(std::string_view key);

            /**
             * @brief Adds stops to the graph in one direction for a given bus.
             * @param stops The deque of stop names in the bus route.
             * @param bus_name The name of the bus.
             */
            void AddStopsOneDirection(const std::deque<std::string_view>& stops, const std::string& bus_name);

            /**
             * @brief Adds stops to the graph in both directions for a given bus.
             * @param stops The deque of stop names in the bus route.
             * @param bus_name The name of the bus.
             */
            void AddStopsNonRoundTrip(std::deque<std::string_view> stops, const std::string& bus_name);
	};
} // namespace graph