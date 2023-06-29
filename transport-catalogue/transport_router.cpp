/**
 * @file transport_router.cpp
 *
 * @brief This file contains the implementation of the ActivityProcessor class for route calculation and related functions.
 */

#include "transport_router.h"
#include <optional>

namespace graph {

		const double MINUTES_PER_KILOMETER = 1000.0 / 60.0;

        /**
         * @brief Constructs an ActivityProcessor object.
         *
         * This constructor initializes the ActivityProcessor with a reference to the TransportCatalogue.
         * It creates a DirectedWeightedGraph and adds knots based on the stops in the TransportCatalogue.
         * It also creates a Router object for route calculation using the created graph.
         *
         * @param tc The TransportCatalogue reference.
         */
		ActivityProcessor::ActivityProcessor(transport_catalogue::TransportCatalogue& tc)
			: tc(tc) {
			graph_ = DirectedWeightedGraph<double>(2 * tc.GetStopsQuantity());
			AddKnots();

			router_ = std::unique_ptr<graph::Router<double>>(new graph::Router<double>(graph_));
		}

        /**
         * @brief Adds knots to the graph based on the stops in the TransportCatalogue.
         *
         * This function iterates through the buses in the TransportCatalogue and adds stops as knots to the graph.
         * It distinguishes between round-trip and non-round-trip buses and adds the stops accordingly.
         */
		void ActivityProcessor::AddKnots() {
			const std::deque<domain::Bus>& buses_ = tc.GetBuses();

			for (const domain::Bus& bus : buses_) {
				std::string type = bus.type;
				std::deque<std::string_view> stops = bus.stops;

				if (type == "true") {
					AddStopsOneDirection(stops, bus.bus_name);
				}
				else {
					AddStopsNonRoundTrip(stops, bus.bus_name);
				}

			}
		}

        /**
         * @brief Calculates the route and buses between two stops.
         *
         * This function calculates the route and buses between the specified starting and destination stops.
         * It uses the Router to find the shortest route in the graph.
         * The result includes a vector of variant types representing bus activities and waiting activities in the route.
         *
         * @param stop_name_from The name of the starting stop.
         * @param stop_name_to The name of the destination stop.
         * @return An optional DestinationInfo structure with the calculated route and buses, or std::nullopt if the stops are not found.
         */
		std::optional<DestinationInfo> ActivityProcessor::GetRouteAndBuses(std::string_view stop_name_from, std::string_view stop_name_to) {
			DestinationInfo dest_info;
			std::vector<std::variant<graph::BusActivity, graph::WaitingActivity>> final_route;
			size_t from;
			size_t to;
			if (!ChekExistValue(stop_name_from) || !ChekExistValue(stop_name_to)) {
				return std::nullopt;
			}

			from = stop_to_vertex_.find(stop_name_from)->second;
			to = stop_to_vertex_.find(stop_name_to)->second;

			std::optional<typename graph::Router<double>::RouteInfo> route_info = router_->BuildRoute(from, to);

			double wait_time = tc.GetWaitTime();

			std::set<std::string> actual_buses;
			std::set<std::string> buses_for_route_first_stop;
			std::set<std::string> buses_for_route_second_stop;

			if (route_info.has_value()) {
				const auto& route_info_value = route_info.value();

				for (auto it = route_info_value.edges.begin(); it != route_info_value.edges.end(); ++it) {
					auto EdgId = *it;
					auto Edge = graph_.GetEdge(EdgId);
					if (Edge.stop_count == 0) {
						WaitingActivity wa;
						wa.time = wait_time;
						wa.stop_name_from = Edge.name;

						final_route.push_back(wa);
						dest_info.all_time += wait_time;

					}

					else {
						BusActivity ba;
						ba.bus_name = Edge.name;
						ba.time = Edge.weight;
						ba.span_count = Edge.stop_count;
						final_route.push_back(ba);
						dest_info.all_time += Edge.weight;

					}

				}
				dest_info.route = final_route;

				return dest_info;
			}
			else {
				return std::nullopt;
			}

		}

        /**
         * @brief Retrieves the value associated with a key in the stop_to_vertex_ map.
         *
         * This function retrieves the value associated with a key in the stop_to_vertex_ map,
         * which represents the vertex index in the graph for a given stop name.
         *
         * @param key The stop name.
         * @return An optional size_t value representing the vertex index, or std::nullopt if the key is not found.
         */
		std::optional<size_t> ActivityProcessor::GetValueByKey(std::string_view key) {
			auto it = stop_to_vertex_.find(key);
			if (it != stop_to_vertex_.end()) {
				return it->second;
			}
			else {
				return std::nullopt;
			}
		}

        /**
         * @brief Checks if a stop exists in the stop_to_vertex_ map.
         *
         * This function checks if a stop exists in the stop_to_vertex_ map by searching for the specified key.
         *
         * @param key The stop name.
         * @return True if the stop exists, False otherwise.
         */
		bool ActivityProcessor::ChekExistValue(std::string_view key) {
			auto it = stop_to_vertex_.find(key);
			if (it != stop_to_vertex_.end()) {
				return true;
			}
			else {
				return false;
			}
		}

        /**
         * @brief Adds stops to the graph in one direction for a given bus.
         *
         * This function adds stops to the graph in one direction for a given bus.
         * It creates vertex indices for the stops and connects them with edges representing the bus route.
         *
         * @param stops The deque of stop names in the bus route.
         * @param bus_name The name of the bus.
         */
		void ActivityProcessor::AddStopsOneDirection(const std::deque<std::string_view>& stops, const std::string& bus_name) {

			for (auto it = stops.begin(); std::next(it) != stops.end(); ++it) {
				double sum_time = 0;
				size_t num_vertex_1_wait;
				size_t num_vertex_next_wait;

				if (ChekExistValue(*it)) {
					auto indx = stop_to_vertex_.find(*it);
					num_vertex_1_wait = indx->second;
				}
				else {
					num_vertex_1_wait = stop_to_vertex_.size() * 2;
					stop_to_vertex_.insert({ *it , num_vertex_1_wait });

				}

				if (ChekExistValue(*std::next(it))) {
					auto indx = stop_to_vertex_.find(*std::next(it));
					num_vertex_next_wait = indx->second;
				}
				else {
					num_vertex_next_wait = stop_to_vertex_.size() * 2;
					stop_to_vertex_.insert({ *std::next(it), num_vertex_next_wait });

				}



				size_t num_vertex1_go = num_vertex_1_wait + 1;


				const domain::Stop* stop_1 = tc.FindStop(*it);
				const domain::Stop* stop_1_next = tc.FindStop(*std::next(it));
				int distance_inner = tc.GetStopDistance(*stop_1, *stop_1_next);
				double time_inner = distance_inner / (tc.GetVelocity() * MINUTES_PER_KILOMETER) + sum_time;

				Edge<double>* edge_mirror = new Edge<double>{ num_vertex_1_wait, num_vertex1_go, tc.GetWaitTime() , std::string(*it), 0 }; 
				Edge<double>* edge_go_wait = new Edge<double>{ num_vertex1_go, num_vertex_next_wait, time_inner, bus_name, 1 };

				graph_.AddEdge(*edge_mirror);

				graph_.AddEdge(*edge_go_wait);

				sum_time = time_inner;

				for (auto it_inner = std::next(it); std::next(it_inner) != stops.end(); ++it_inner) {

					size_t num_vertex_inner_next_wait;
					if (ChekExistValue(*std::next(it_inner))) {
						auto indx = stop_to_vertex_.find(*std::next(it_inner));
						num_vertex_inner_next_wait = indx->second;
					}
					else {
						num_vertex_inner_next_wait = stop_to_vertex_.size() * 2;
						stop_to_vertex_.insert({ *std::next(it_inner), num_vertex_inner_next_wait });

					}



					const domain::Stop* stop_inner = tc.FindStop(*it_inner);
					const domain::Stop* stop_inner_next = tc.FindStop(*(std::next(it_inner)));
					int distance_1_2 = tc.GetStopDistance(*stop_inner, *stop_inner_next);


					double time_min_1_2 = distance_1_2 / (tc.GetVelocity() * MINUTES_PER_KILOMETER) + sum_time;

					int span_count = std::distance(stops.begin(), std::next(it_inner)) - std::distance(stops.begin(), it);

					Edge<double>* edge_go_wait_inner = new Edge<double>{ num_vertex1_go, num_vertex_inner_next_wait , time_min_1_2, bus_name, span_count };

					graph_.AddEdge(*edge_go_wait_inner);
					sum_time = time_min_1_2;

				}
			}

		}

        /**
         * @brief Adds stops to the graph in both directions for a given bus.
         *
         * This function adds stops to the graph in both directions for a given bus.
         * It first adds the stops in one direction and then reverses the order and adds them again.
         *
         * @param stops The deque of stop names in the bus route.
         * @param bus_name The name of the bus.
         */
		void ActivityProcessor::AddStopsNonRoundTrip(std::deque<std::string_view> stops, const std::string& bus_name) {  // Тут если передать stops по константной ссылке то ошибка в utility
			AddStopsOneDirection(stops, bus_name);
			std::reverse(stops.begin(), stops.end());
			AddStopsOneDirection(stops, bus_name);

		}
	
} // namespace graph