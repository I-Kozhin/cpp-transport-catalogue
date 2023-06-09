#pragma once

/**
 * @file map_renderer.h
 * @brief This file contains the declaration of the MapRenderer class, which is responsible for rendering a map with routes.
 */

#include "svg.h"
#include "transport_catalogue.h"
#include "domain.h"
#include "geo.h"
#include "json.h"
#include <algorithm>


namespace transport_catalogue {

    /**
     * @struct Renderdata
     * @brief A structure that holds the rendering data for the map.
     */
    struct RenderData {

        double width_;
        double height_;
        double padding_;
        double line_width_;
        double stop_radius_;
        int bus_label_font_size_;
        std::pair<double, double> bus_label_offset_;
        int stop_label_font_size_;
        std::pair<double, double> stop_label_offset_;
        svg::Color underlayer_color_;
        double underlayer_width_;
        std::vector<svg::Color> color_palette_;

    };


    /**
     * @brief Checks if the given value is zero.
     * @param value The value to check.
     * @return True if the value is zero, false otherwise.
     */
    bool IsZero(double value);

    /**
     * @class SphereProjector
     * @brief A class that projects geographic coordinates onto a sphere to create a map 
     */
    class SphereProjector {
        public:

            /**
             * @brief Constructs a SphereProjector object.
             * @param points_begin An iterator pointing to the beginning of the points range.
             * @param points_end An iterator pointing to the end of the points range.
             * @param max_width The maximum width of the map.
             * @param max_height The maximum height of the map.
             * @param padding The padding around the map.
             */
            template <typename PointInputIt>
            SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                double max_width, double max_height, double padding)
                : padding_(padding)
            {
                // If there are no points, return
                if (points_begin == points_end) {
                    return;
                }

                // Find the minimum and maximum longitude values
                const auto [left_it, right_it] = std::minmax_element(
                    points_begin, points_end,
                    [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
                min_lon_ = left_it->lng;
                const double max_lon = right_it->lng;

                // Find the minimum and maximum latitude values
                const auto [bottom_it, top_it] = std::minmax_element(
                    points_begin, points_end,
                    [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
                const double min_lat = bottom_it->lat;
                max_lat_ = top_it->lat;

                // Calculate the zoom coefficients for width and height
                std::optional<double> width_zoom;
                if (!IsZero(max_lon - min_lon_)) {
                    width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
                }


                std::optional<double> height_zoom;
                if (!IsZero(max_lat_ - min_lat)) {
                    height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
                }

                if (width_zoom && height_zoom) {

                    zoom_coeff_ = std::min(*width_zoom, *height_zoom);
                }
                else if (width_zoom) {

                    zoom_coeff_ = *width_zoom;
                }
                else if (height_zoom) {

                    zoom_coeff_ = *height_zoom;
                }
            }

            /**
             * @brief Projects geographic coordinates onto the sphere to get the corresponding point on the map.
             * @param coords The geographic coordinates to project.
             * @return The corresponding point on the map.
             */
            svg::Point operator()(geo::Coordinates coords) const {
                return {
                    (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                    (max_lat_ - coords.lat) * zoom_coeff_ + padding_
                };
            }

        private:
            double padding_;
            double min_lon_ = 0;
            double max_lat_ = 0;
            double zoom_coeff_ = 0;
    };

    /**
     * @class MapRenderer
     * @brief A class that renders a map with routes.
     */
    class MapRenderer {
        public:

            /**
             * @brief Constructs a MapRenderer object.
             * @param render_data The rendering data for the map.
             */
            explicit MapRenderer(const RenderData& render_data);

            /**
             * @brief Draws the routes on the map and returns the SVG document as a string.
             * @param tc The transport catalogue containing the route information.
             * @return The SVG document as a string.
             */
            std::string DrawRouteGetDoc(const transport_catalogue::TransportCatalogue& tc);


        private:
            const RenderData& map_render_data_;

            std::deque<domain::Bus> GetSortedBuses(const transport_catalogue::TransportCatalogue& tc);

            void DrawRoutes(const transport_catalogue::TransportCatalogue& tc, std::deque<domain::Bus>& buses, const SphereProjector& proj_one,
        std::map<std::string, svg::Color>& colors, std::vector<svg::Text>& routes_text, std::vector<svg::Polyline>& routes_vec);

            void DrawStops(const transport_catalogue::TransportCatalogue& tc, const SphereProjector& proj_one,
        const std::set<std::string>& stops_for_route, std::vector<svg::Text>& stops_names,
        std::vector<svg::Circle>& stops_circles);
    };
}