#include "map_renderer.h"

/**
 * @file map_renderer.h
 * @brief This file contains the implementation of the MapRenderer class and related functions for drawing routes and stops on a map.
 */

using namespace std::literals;
using namespace svg;
using namespace std;
using namespace domain;
using namespace transport_catalogue;


struct RoutDescription {
    uint32_t x, y; // x and y coordinates of the corresponding stop
    geo::Coordinates last_coords; // coordinates of the last stop if it exists and not equal to the first one
    std::string font = "Verdana"; // Font name
    std::string font_weight = "bold"; // Font weight
    std::string text_data; // Text content of the object
    std::string_view last_stop; // Text content of the object
    Color route_color;
};

/**
 * @brief Creates a Polyline object from a vector of points.
 * @param points The vector of points to create the Polyline from.
 * @return The created Polyline object.
 */
svg::Polyline CreatePolyline(const vector<svg::Point>& points) {
    using namespace svg;
    Polyline polyline;
    for (const auto& p : points) { 
        polyline.AddPoint(p); 
    }

    return polyline;
}

inline const double EPSILON = 1e-6;

/**
 * @brief Checks if a given double value is close to zero within a small epsilon.
 * @param value The double value to check.
 * @return True if the value is close to zero, false otherwise.
 */
bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}


/**
 * @brief Gets the color for each route based on a deque of buses and a color palette.
 * @param buses The deque of buses.
 * @param color_palette The color palette to choose from.
 * @return A map associating each bus route name with its corresponding color.
 */
std::map<string, Color> GetColorForRoute(const std::deque<Bus>& buses, std::vector<Color> color_palette) {
    std::map<string, Color> result;
    int index_color = 0;
    for (const auto& bus : buses) {
        if (bus.stops.size() != 0) {
            if (index_color > color_palette.size() - 1) { index_color = 0; }

            result[bus.bus_name] = color_palette[index_color];
            ++index_color;
        }

    }
    return result;

}


/**
 * @brief Returns the stops in reverse order, excluding the last stop, to create non-round-trip routes.
 * @param stops The deque of stops.
 * @return The reversed deque of stops.
 */
deque<string_view> GetStopsForNonRounTtip(deque<string_view> stops) {
    deque<string_view> reverse_stops;
    reverse_stops.insert(reverse_stops.end(), stops.begin(), stops.end());
    std::reverse(stops.begin(), stops.end());
    stops.pop_front();
    reverse_stops.insert(reverse_stops.end(), stops.begin(), stops.end());

    return reverse_stops;

}


/**
 * @brief Returns a vector of all coordinates from the TransportCatalogue for the given buses.
 * @param tc The TransportCatalogue object.
 * @param buses The deque of buses.
 * @return The vector of all coordinates for the given buses.
 */
vector<geo::Coordinates> GetAllCoordinates(const TransportCatalogue& rtotc, const std::deque<Bus> buses) {
    vector<geo::Coordinates> geo_coords;
    for (const auto& bus : buses) {
        std::deque<std::string_view> current_stops;
        if (bus.type != "true") {
            current_stops = GetStopsForNonRounTtip(bus.stops);
        }
        else { current_stops = bus.stops; }
        for (int i = 0; i < current_stops.size(); i++) {
            const Stop* one = rtotc.FindStop(current_stops[i]);
            geo_coords.push_back(one->coordinates);
        }
    }
    return geo_coords;
}


/**
 * @brief Checks if the current stops of a route are the same (round-trip route).
 * @param current_stops The deque of current stops for a route.
 * @return True if the current stops are the same, false otherwise.
 */
bool  CheckSameStations(const deque<string_view>& current_stops) {
    if (current_stops.front() == current_stops.back()) {
        return true;
    }
    else {
        return false;
    }
}


/**
 * @brief Constructs a MapRenderer object with the given RenderData.
 * @param render_data The RenderData object containing rendering settings.
 */
MapRenderer::MapRenderer(RenderData& render_data)
    :map_render_data_(render_data)
{
   
}

// декомпозиция 1 Получение и сортировка автобусов
std::deque<domain::Bus> MapRenderer::GetSortedBuses(const transport_catalogue::TransportCatalogue& tc) {
    std::deque<domain::Bus> buses = tc.GetBuses();
    std::sort(buses.begin(), buses.end(), [](const domain::Bus& a, const domain::Bus& b) {
        return a.bus_name < b.bus_name;
    });

    return buses;
}

// декомпозиция 2 отрисовка маршрутов 
void MapRenderer::DrawRoutes(const transport_catalogue::TransportCatalogue& tc, std::deque<domain::Bus>& buses, const SphereProjector& proj_one,
    std::map<std::string, svg::Color>& colors, std::vector<svg::Text>& routes_text, std::vector<svg::Polyline>& routes_vec) {
    for (const auto& bus : buses) {
        if (bus.stops.size() == 0) {
            string empty_doc;
            return;
        }

        bool same_stations = true;
        RoutDescription rout_description;
        rout_description.text_data = bus.bus_name;
        vector<std::pair<string*, svg::Point>> route_stations;


        Color current_color;
        deque<string_view> current_stops;
        if (colors.count(bus.bus_name)) {
            {
                current_color = colors[bus.bus_name];
            }

        }

        rout_description.route_color = current_color;
        rout_description.last_stop = bus.stops.back();

        vector<svg::Point> point_to_draw;

        if (bus.type != "true") {
            current_stops = GetStopsForNonRounTtip(bus.stops);
            same_stations = CheckSameStations(bus.stops);
        }
        else {
            current_stops = bus.stops;
        }

        for (int i = 0; i < current_stops.size(); i++) {
            const Stop* one = tc.FindStop(current_stops[i]);
            const svg::Point screen_coord = proj_one(one->coordinates);
            svg::Point p;
            p.x = screen_coord.x;
            p.y = screen_coord.y;

            if (i == 0) {
                Text route_font;
                Text route;

                route_font.SetPosition(Point(screen_coord.x, screen_coord.y)).SetOffset(Point(map_render_data_.bus_label_offset[0], map_render_data_.bus_label_offset[1])).SetFontSize(map_render_data_.bus_label_font_size)\
                    .SetFontFamily(rout_description.font).SetFontWeight(rout_description.font_weight).SetData(rout_description.text_data)\
                    .SetFillColor(map_render_data_.underlayer_color[0]).SetStrokeColor(map_render_data_.underlayer_color[0]).SetStrokeWidth(map_render_data_.underlayer_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);

                route.SetPosition(Point(screen_coord.x, screen_coord.y)).SetOffset(Point(map_render_data_.bus_label_offset[0], map_render_data_.bus_label_offset[1])).SetFontSize(map_render_data_.bus_label_font_size)\
                    .SetFontFamily(rout_description.font).SetFontWeight(rout_description.font_weight).SetData(rout_description.text_data).SetFillColor(rout_description.route_color);

                routes_text.push_back(route_font);
                routes_text.push_back(route);
            }

            point_to_draw.push_back(p);
        }

        if (!same_stations) {
            const Stop* last = tc.FindStop(rout_description.last_stop);
            const svg::Point screen_coord_last = proj_one(last->coordinates);
            Text route_font_not_same;
            Text route_not_same;

            route_font_not_same.SetPosition(Point(screen_coord_last.x, screen_coord_last.y)).SetOffset(Point(map_render_data_.bus_label_offset[0], map_render_data_.bus_label_offset[1])).SetFontSize(map_render_data_.bus_label_font_size)\
                .SetFontFamily(rout_description.font).SetFontWeight(rout_description.font_weight).SetData(rout_description.text_data)\
                .SetFillColor(map_render_data_.underlayer_color[0]).SetStrokeColor(map_render_data_.underlayer_color[0]).SetStrokeWidth(map_render_data_.underlayer_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);

            route_not_same.SetPosition(Point(screen_coord_last.x, screen_coord_last.y)).SetOffset(Point(map_render_data_.bus_label_offset[0], map_render_data_.bus_label_offset[1])).SetFontSize(map_render_data_.bus_label_font_size)\
                .SetFontFamily(rout_description.font).SetFontWeight(rout_description.font_weight).SetData(rout_description.text_data).SetFillColor(rout_description.route_color);

            routes_text.push_back(route_font_not_same);
            routes_text.push_back(route_not_same);
        }

        svg::Document  doc;
        Polyline polyline = CreatePolyline(point_to_draw);
        polyline.SetFillColor(NoneColor).SetStrokeColor(current_color).SetStrokeWidth(map_render_data_.line_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);

        routes_vec.push_back(polyline);
    }
}



/**
 * @brief Draws the routes and stops on a map and returns the SVG document as a string.
 * @param tc The TransportCatalogue object.
 * @return The SVG document as a string.
 */
std::string MapRenderer::DrawRouteGetDoc(const TransportCatalogue& tc) {
    // Инициализация векторов и переменных
    vector<Text> stops_names;
    vector<Circle> stops_circles;
    vector<svg::Document> docs;
    vector<svg::Polyline> routes_vec;
    vector<svg::Text> routes_text;
    std::vector<Color> color_palette = map_render_data_.color_palette;
    std::deque<Stop> stops = tc.GetStops();
    // декомпозиция 1 
    std::deque<Bus> buses = GetSortedBuses(tc);

    std::map<string, Color> colors = GetColorForRoute(buses, color_palette);
    vector<geo::Coordinates> geo_coords = GetAllCoordinates(tc, buses);

    const SphereProjector proj_one{
        geo_coords.begin(), geo_coords.end(), map_render_data_.width, map_render_data_.height, map_render_data_.padding
    };

    std::set<std::string> stops_for_route;

    for (const auto& el : stops) {
        if (tc.GetStopInfo(el.stop_name).size() != 0) {
            stops_for_route.insert(el.stop_name);
        }
    }

    // декомпозиция 2 Отрисовка маршрутов
    DrawRoutes(tc, buses, proj_one, colors, routes_text,routes_vec);

    // Отрисовка остановок и текста для маршрутов
    for (auto i = stops_for_route.begin(); i != stops_for_route.end(); ++i) {

        Circle c;
        const Stop* one = tc.FindStop(*i);
        const svg::Point screen_coord = proj_one(one->coordinates);


        c.SetRadius(map_render_data_.stop_radius).SetCenter({ screen_coord.x, screen_coord.y });
        c.SetFillColor("white");
        stops_circles.push_back(c);



        Text stop_description_font;
        Text stop_description;

        stop_description_font.SetPosition(Point(screen_coord.x, screen_coord.y)).SetOffset(Point(map_render_data_.stop_label_offset[0], map_render_data_.stop_label_offset[1])).SetFontSize(map_render_data_.stop_label_font_size)\
            .SetFontFamily("Verdana").SetData(*i)\
            .SetFillColor(map_render_data_.underlayer_color[0]).SetStrokeColor(map_render_data_.underlayer_color[0]).SetStrokeWidth(map_render_data_.underlayer_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);


        stop_description.SetPosition(Point(screen_coord.x, screen_coord.y)).SetOffset(Point(map_render_data_.stop_label_offset[0], map_render_data_.stop_label_offset[1])).SetFontSize(map_render_data_.stop_label_font_size)\
            .SetFontFamily("Verdana").SetData(*i).SetFillColor("black");

        stops_names.push_back(stop_description_font);
        stops_names.push_back(stop_description);

    }

    // Создание SVG-документа
    svg::Document  doc;
    for (const auto& polyline : routes_vec) {
        doc.Add(std::move(polyline));
    }

    for (const auto& text : routes_text) {
        doc.Add(std::move(text));
    }



    for (const auto& c : stops_circles) {
        doc.Add(std::move(c));
    }

    for (const auto& stop : stops_names) {
        doc.Add(std::move(stop));
    }

    std::ostringstream os;

    doc.Render(os);
    return os.str();


}