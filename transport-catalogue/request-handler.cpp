
/**
 * @file request_handler.cpp
 * @brief Contains the implementation of the RequestHandler class.  
 */

#include "request_handler.h"
#include <sstream>
#include <iostream>

using namespace transport_catalogue;

/**
 * @brief Constructs a RequestHandler object.
 * @param tc A reference to the TransportCatalogue object.
 * @param renderer A reference to the MapRenderer object
 */
RequestHandler::RequestHandler( TransportCatalogue& tc, MapRenderer& renderer) : transport_catalogue_(tc), renderer_(renderer) {}

/**
 * @brief Renders a map of the transport data using a string-based configuration.
 */
void RequestHandler::RenderMapByString() {
    std::string str = renderer_.DrawRouteGetDoc(transport_catalogue_);
   
    std::stringstream ss;
    ss << str;
    std::string result = ss.str();
    std::cout << result << std::endl; 
   
}