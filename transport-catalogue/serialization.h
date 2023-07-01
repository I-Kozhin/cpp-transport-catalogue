#pragma once

/**
 * @file serialization.h
 * @brief This file contains the declarations of functions and structures for serialization and deserialization of the Transport Catalogue.
 */

#include "transport_catalogue.h"
#include "transport_catalogue.pb.h"

#include "svg.pb.h"

#include "map_renderer.h"
#include "map_renderer.pb.h"

#include "transport_router.h"
#include "transport_router.pb.h"

#include <iostream>

namespace serialization {

    /**
     * @struct SerializationSettings
     * @brief Struct containing serialization settings.
     */
    struct SerializationSettings {std::string file_name;};

    /**
     * @struct Catalogue
     * @brief Struct containing the Transport Catalogue and related settings for serialization.
     */
    struct Catalogue {
        transport_catalogue::TransportCatalogue transport_catalogue_;
        transport_catalogue::RenderData render_settings_;
        domain::RouteSettings routing_settings_;
    };

    /**
     * @brief Calculates the ID based on a range of elements and a name.
     * @tparam It The type of iterator.
     * @param start The start iterator of the range.
     * @param end The end iterator of the range.
     * @param name The name for which the ID is calculated.
     * @return The calculated ID.
     */
    template <typename It>
    uint32_t calculate_id(It start, It end, std::string_view name);

    /**
     * @brief Serializes the Transport Catalogue into a protobuf object.
     * @param transport_catalogue The Transport Catalogue object.
     * @return The serialized Transport Catalogue protobuf object.
     */
    transport_catalogue_protobuf::TransportCatalogue transport_catalogue_serialization(const transport_catalogue::TransportCatalogue& transport_catalogue);
    
    /**
     * @brief Deserializes the Transport Catalogue from a protobuf object.
     * @param transport_catalogue_proto The serialized Transport Catalogue protobuf object.
     * @return The deserialized Transport Catalogue object.
     */
    transport_catalogue::TransportCatalogue transport_catalogue_deserialization(const transport_catalogue_protobuf::TransportCatalogue& transport_catalogue_proto);

    /**
     * @brief Serializes the Color object into a protobuf object.
     * @param tc_color The Color object to serialize.
     * @return The serialized Color protobuf object.
     */
    transport_catalogue_protobuf::Color color_serialization(const svg::Color& tc_color);

    /**
     * @brief Deserializes the Color object from a protobuf object.
     * @param color_proto The serialized Color protobuf object.
     * @return The deserialized Color object.
     */
    svg::Color color_deserialization(const transport_catalogue_protobuf::Color& color_proto);

    /**
     * @brief Serializes the RenderData object into a protobuf object.
     * @param render_settings The RenderData object to serialize.
     * @return The serialized RenderSettings protobuf object.
     */
    transport_catalogue_protobuf::RenderSettings render_settings_serialization(const transport_catalogue::RenderData& render_settings);

    /**
     * @brief Deserializes the RenderData object from a protobuf object.
     * @param render_settings_proto The serialized RenderSettings protobuf object.
     * @return The deserialized RenderData object.
     */
    transport_catalogue::RenderData render_settings_deserialization(const transport_catalogue_protobuf::RenderSettings& render_settings_proto);

    /**
     * @brief Serializes the RouteSettings object into a protobuf object.
     * @param routing_settings The RouteSettings object to serialize.
     * @return The serialized RouteSettings protobuf object.
     */
    transport_catalogue_protobuf::RouteSettings routing_settings_serialization(const domain::RouteSettings& routing_settings);

    /**
     * @brief Deserializes the RouteSettings object from a protobuf object.
     * @param routing_settings_proto The serialized RouteSettings protobuf object.
     * @return The deserialized RouteSettings object.
     */
    domain::RouteSettings routing_settings_deserialization(const transport_catalogue_protobuf::RouteSettings& routing_settings_proto);

    /**
     * @brief Serializes the Transport Catalogue, RenderData, and RouteSettings into a stream.
     * @param transport_catalogue The Transport Catalogue object to serialize.
     * @param render_settings The RenderData object to serialize.
     * @param routing_settings The RouteSettings object to serialize.
     * @param out The output stream to write the serialized data to.
     */
    void catalogue_serialization(const transport_catalogue::TransportCatalogue& transport_catalogue,
                                 const transport_catalogue::RenderData& render_settings,
                                 const domain::RouteSettings& routing_settings,
                                 std::ostream& out);

    /**
     * @brief Deserializes the Transport Catalogue, RenderData, and RouteSettings from a stream.
     * @param in The input stream to read the serialized data from.
     * @return The deserialized Catalogue object.
     */
    Catalogue catalogue_deserialization(std::istream& in);

}  // namespace serialization