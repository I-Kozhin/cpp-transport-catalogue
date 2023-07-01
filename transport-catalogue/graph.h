#pragma once

#include "ranges.h"
#include "transport_catalogue.h"

#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string_view>

namespace graph {

    using VertexId = size_t;
    using EdgeId = size_t;

    /**
     * @struct Edge
     * @brief Struct representing an edge in a directed weighted graph.
     * @tparam Weight The weight type of the edge.
     */
    template <typename Weight>
    struct Edge {
        VertexId from;   /**< The source vertex of the edge. */
        VertexId to;     /**< The target vertex of the edge. */
        Weight weight;   /**< The weight of the edge. */
        std::string name;   /**< The name of the edge. */
        int stop_count; /**< The number of stops in the edge. */

        /**
         * @brief Overloaded equality operator for comparing edges.
         * @param other The edge to compare with.
         * @return True if the edges are equal, False otherwise.
         */
        bool operator==(const Edge& other) const {
            return from == other.from && to == other.to && weight == other.weight;
        }
    };

    /**
     * @class DirectedWeightedGraph
     * @brief Class representing a directed weighted graph.
     * @tparam Weight The weight type of the graph.
     */
    template <typename Weight>
    class DirectedWeightedGraph {
        private:
            using IncidenceList = std::vector<EdgeId>;
            using IncidentEdgesRange = ranges::Range<typename IncidenceList::const_iterator>;

        public:
            DirectedWeightedGraph() = default;
            explicit DirectedWeightedGraph(size_t vertex_count);
            EdgeId AddEdge(const Edge<Weight>& edge);

            size_t GetVertexCount() const;
            size_t GetEdgeCount() const;
            const Edge<Weight>& GetEdge(EdgeId edge_id) const;
            IncidentEdgesRange GetIncidentEdges(VertexId vertex) const;

        private:
            std::vector<Edge<Weight>> edges_;
            std::vector<IncidenceList> incidence_lists_;
    };

    template <typename Weight>
    DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertex_count)
        : incidence_lists_(vertex_count) {
    }

    template <typename Weight>
    EdgeId DirectedWeightedGraph<Weight>::AddEdge(const Edge<Weight>& edge) {
        edges_.push_back(edge);
        const EdgeId id = edges_.size() - 1;
        incidence_lists_.at(edge.from).push_back(id); 
        return id;
    }

    template <typename Weight>
    size_t DirectedWeightedGraph<Weight>::GetVertexCount() const {
        return incidence_lists_.size();
    }

    template <typename Weight>
    size_t DirectedWeightedGraph<Weight>::GetEdgeCount() const {
        return edges_.size();
    }

    template <typename Weight>
    const Edge<Weight>& DirectedWeightedGraph<Weight>::GetEdge(EdgeId edge_id) const {
        return edges_.at(edge_id);
    }

    template <typename Weight>
    typename DirectedWeightedGraph<Weight>::IncidentEdgesRange
        DirectedWeightedGraph<Weight>::GetIncidentEdges(VertexId vertex) const {
        return ranges::AsRange(incidence_lists_.at(vertex));
    }



}  // namespace graph  