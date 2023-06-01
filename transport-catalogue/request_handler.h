#pragma once

/**
 * @file request_handler.h
 * @brief Contains the declaration of the RequestHandler class.
 */

#include "transport_catalogue.h"
#include "svg.h"
#include "map_renderer.h"

/**
 * @class RequestHandler
 * @brief Handles requests and provides various functionalities related to transport data.
 * The RequestHandler class is responsible for handling requests and providing
 * functionalities related to transport data, such as retrieving bus statistics,
 * getting buses by stop, and rendering maps.
 */
class RequestHandler {
    public:
        
        /**
         * @brief Constructs a RequestHandler object.
         * @param tc A reference to the TransportCatalogue object.
         * @param renderer A reference to the MapRenderer object.
         */
        RequestHandler( transport_catalogue::TransportCatalogue& tc, MapRenderer& renderer);

        /**
         * @brief Renders a map of the transport data using a string-based configuration.
         */
        void RenderMapByString();

    private:
        
        transport_catalogue::TransportCatalogue& transport_catalogue_;   ///< Reference to the TransportCatalogue 
        MapRenderer& renderer_;                         ///< Reference to the MapRenderer object.
};