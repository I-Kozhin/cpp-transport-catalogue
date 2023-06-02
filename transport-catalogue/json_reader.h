#pragma once

/**
 * @file json_reader.h
 * @brief This file contains the declaration of the InputReaderJson class for reading input data from JSON format.
 */

#include <sstream>
#include <string>
#include <deque>
#include <iostream>
#include <vector>

#include "transport_catalogue.h"
#include "domain.h"
#include "json.h"
#include "geo.h"
#include "map_renderer.h"
#include "json_builder.h"



inline json::Document LoadJSON(const std::string& s) {
    std::istringstream strm(s);
    return json::Load(strm);
}

inline std::string Print(const json::Node& node) {
    std::ostringstream out;
    Print(json::Document{ node }, out);
    return out.str();
}


namespace transport_catalogue {

    /**
     * @class InputReaderJson
     * @brief Class for reading input data from JSON format.
     */
    class InputReaderJson {
        public:

            /**
            * @brief Constructs an InputReaderJson object with the given input stream.
            * @param is The input stream to read from.
            */
            explicit InputReaderJson(std::istream& is);

            void ReadInputJsonBaseRequest();
            void ReadInputJsonStatRequest();
            void ReadInputJsonRenderSettings();

            void ReadInputJsonRequest();

            /**
             * @brief Updates the stops in the transport catalogue.
             * @param tc The transport catalogue to update.
             */
            void UpdStop(TransportCatalogue& tc);

            /**
             * @brief Updates the buses in the transport catalogue.
             * @param tc The transport catalogue to update.
             */
            void UpdBus(TransportCatalogue& tc);

            /**
             * @brief Updates the stop distances in the transport catalogue.
             * @param tc The transport catalogue to update.
             */
            void UpdStopDist(TransportCatalogue& tc);

            /**
            * @brief Manages the output requests for the transport catalogue and map renderer.
            * @param tc The transport catalogue.
            * @param mr The map renderer.
            */
            void ManageOutputRequests(TransportCatalogue& tc, MapRenderer& mr);

            RenderData GetRenderData();



        private:

            std::istream& input_stream_;
            std::deque<OutputRequest> output_requests_;
            std::deque<domain::BusDescription> update_requests_bus_;
            std::deque<domain::Stop> update_requests_stop_;
            std::vector<domain::StopDistancesDescription> distances_;   ///< The stop distances.
            RenderData render_data_;
            json::Document load_;   ///< The loaded JSON document.

    };

}  // namespace transport_catalogue
