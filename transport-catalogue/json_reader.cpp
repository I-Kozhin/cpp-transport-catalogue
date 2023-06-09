/**
 * @file json_reader.cpp
 * @brief Implementation of the InputReaderJson class for reading input data from JSON format.
 */

#include "transport_catalogue.h"
#include "json.h"
#include "geo.h"
#include "svg.h"
#include "json_reader.h"
#include "json_builder.h"


using namespace json;
using namespace std;
using namespace domain;
using namespace svg;

namespace transport_catalogue {

	/**
	 * @brief Gets the color from a JSON node.
	 * @param el The JSON node representing the color.
	 * @return The color extracted from the JSON node.
	 */
	svg::Color GetColor(const json::Node& el) {
		svg::Color color;

		if (el.IsString()) {
			color = el.AsString();
		}
		else {
			if (el.AsArray().size() == 3) {
				Rgb rgb;
				rgb.red_ = static_cast<uint8_t>(el.AsArray()[0].AsInt());
				rgb.green_ = static_cast<uint8_t>(el.AsArray()[1].AsInt());
				rgb.blue_ = static_cast<uint8_t>(el.AsArray()[2].AsInt());
				color = rgb;
			}
			else if (el.AsArray().size() == 4) {
				Rgba rgba;
				rgba.red_ = static_cast<uint8_t>(el.AsArray()[0].AsInt());
				rgba.green_ = static_cast<uint8_t>(el.AsArray()[1].AsInt());
				rgba.blue_ = static_cast<uint8_t>(el.AsArray()[2].AsInt());
				rgba.opacity_ = el.AsArray()[3].AsDouble();
				color = rgba;
			}
		}


		return color;

	}

	/**
	 * @brief Constructs an InputReaderJson object with the input stream.
	 * @param is The input stream.
	 */
	InputReaderJson::InputReaderJson(istream& is) : input_stream_(is), load_(json::Load(is)) {

	}

	/**
	 * @brief Reads the base requests from the JSON input.
	 */
	void InputReaderJson::ReadInputJsonBaseRequest() {
		const auto& json_array = ((load_.GetRoot()).AsDict()).at("base_requests"s);
		for (const auto& file : json_array.AsArray()) {
			const auto& json_obj = file.AsDict();
			if (json_obj.at("type"s) == "Stop"s) {
				Stop stopjson;
				stopjson.stop_name = json_obj.at("name").AsString();
				stopjson.coordinates.lat = json_obj.at("latitude").AsDouble();
				stopjson.coordinates.lng = json_obj.at("longitude").AsDouble();

				StopDistancesDescription input_stop_dist;
				input_stop_dist.stop_name = json_obj.at("name").AsString();
				auto heighbors = json_obj.at("road_distances");

				for (auto el : heighbors.AsDict()) {
					input_stop_dist.distances.push_back(make_pair(el.first, el.second.AsInt()));

				}
				update_requests_stop_.push_back(stopjson);
				distances_.push_back(input_stop_dist);
			}
			else if (json_obj.at("type"s) == "Bus"s) {
				BusDescription bs;
				auto stop_list = json_obj.at("stops").AsArray();
				for (auto el : stop_list) {

					bs.stops.push_back(el.AsString());

				}
				bs.bus_name = json_obj.at("name").AsString();

				auto trip = json_obj.at("is_roundtrip").AsBool();
				if (trip) {
					bs.type = "true"s;
				}
				else { bs.type = "false"s; }

				update_requests_bus_.push_back(bs);

			}
		}

	}

	/**
	 * @brief Reads the stat requests from the JSON input.
	 */
	void InputReaderJson::ReadInputJsonStatRequest() {
		const auto& json_array_out = ((load_.GetRoot()).AsDict()).at("stat_requests"s);
		if (!json_array_out.IsNull()) {
			for (const auto& file : json_array_out.AsArray()) {
				const auto& json_obj = file.AsDict();
				OutputRequest outputstopjson;
				if (json_obj.at("type").AsString() == "Map"s) {
					outputstopjson.id = json_obj.at("id").AsInt();
					outputstopjson.type = json_obj.at("type").AsString();
					output_requests_.push_back(outputstopjson);

				} else if (json_obj.at("type").AsString() == "Route"s) {
					outputstopjson.id = json_obj.at("id").AsInt();
					outputstopjson.type = json_obj.at("type").AsString();
					outputstopjson.from = json_obj.at("from").AsString();
					outputstopjson.to = json_obj.at("to").AsString();

					output_requests_.push_back(outputstopjson);
				} 
				else {
					outputstopjson.name = json_obj.at("name").AsString();
					outputstopjson.id = json_obj.at("id").AsInt();
					outputstopjson.type = json_obj.at("type").AsString();
					output_requests_.push_back(outputstopjson);
				}
			}
		}

	}

	/**
	 * @brief Reads the render settings from the JSON input.
	 */
	void InputReaderJson::ReadInputJsonRenderSettings() {

		const auto& json_array_render = ((load_.GetRoot()).AsDict()).at("render_settings"s).AsDict();
		if (json_array_render.find("width") != json_array_render.end()) {
            render_data_.width_ = json_array_render.find("width")->second.AsDouble();
		}

		if (json_array_render.find("height") != json_array_render.end()) {
            render_data_.height_ = json_array_render.find("height")->second.AsDouble();
		}

		if (json_array_render.find("padding") != json_array_render.end()) {
            render_data_.padding_ = json_array_render.find("padding")->second.AsDouble();
		}

		if (json_array_render.find("line_width") != json_array_render.end()) {
            render_data_.line_width_ = json_array_render.find("line_width")->second.AsDouble();
		}

		if (json_array_render.find("stop_radius") != json_array_render.end()) {
            render_data_.stop_radius_ = json_array_render.find("stop_radius")->second.AsDouble();
		}

		if (json_array_render.find("bus_label_font_size") != json_array_render.end()) {
            render_data_.bus_label_font_size_ = json_array_render.find("bus_label_font_size")->second.AsDouble();
		}

        if (json_array_render.find("bus_label_offset") != json_array_render.end()) {

            auto el = json_array_render.find("bus_label_offset")->second.AsArray();
            render_data_.bus_label_offset_ = std::make_pair(el[0].AsDouble(), el[1].AsDouble() );
        }

        if (json_array_render.find("stop_label_offset") != json_array_render.end()) {
            auto el = json_array_render.find("stop_label_offset")->second.AsArray();
            render_data_.stop_label_offset_ = std::make_pair(el[0].AsDouble(), el[1].AsDouble() );

        }


		if (json_array_render.find("stop_label_font_size") != json_array_render.end()) {
            render_data_.stop_label_font_size_ = json_array_render.find("stop_label_font_size")->second.AsDouble();
		}

        if (json_array_render.find("underlayer_color") != json_array_render.end()) {
            render_data_.underlayer_color_ = GetColor(json_array_render.find("underlayer_color")->second);

        }


		if (json_array_render.find("underlayer_width") != json_array_render.end()) {
            render_data_.underlayer_width_ = json_array_render.find("underlayer_width")->second.AsDouble();
		}

		if (json_array_render.find("color_palette") != json_array_render.end()) {
			for (auto el : json_array_render.find("color_palette")->second.AsArray()) {

                render_data_.color_palette_.push_back(GetColor(el));

			}

		}

	}

	/**
	 * @brief Reads the route settings from the JSON input.
	 */
	void InputReaderJson::ReadInputJsonRouteSettings() {
		const auto& json_array_out = ((load_.GetRoot()).AsDict()).at("routing_settings"s);
		const auto& json_obj = json_array_out.AsDict();
		route_settings_.bus_velocity = json_obj.at("bus_velocity").AsDouble();
		route_settings_.bus_wait_time = json_obj.at("bus_wait_time").AsDouble();
	}

	/**
	 * @brief Reads the serialization settings from the JSON input.
	 */
	void InputReaderJson::ReadInputJsonSerializeSettings() {
		const auto& json_array_out = ((load_.GetRoot()).AsDict()).at("serialization_settings"s);
		const auto& json_obj = json_array_out.AsDict();
		serialize_file_path_ = json_obj.at("file").AsString();
	}

	/**
	 * @brief Reads the request information from the JSON input.
	 */
	void InputReaderJson::ReadInputJsonRequest() {
		ReadInputJsonBaseRequest();
		ReadInputJsonRenderSettings();
		ReadInputJsonRouteSettings();
		ReadInputJsonSerializeSettings();
	}

	/**
	 * @brief Reads the request information for filling the base from the JSON input.
	 */
	void InputReaderJson::ReadInputJsonRequestForFillBase() {
		ReadInputJsonBaseRequest();
		ReadInputJsonRenderSettings();
		ReadInputJsonRouteSettings();
		ReadInputJsonSerializeSettings();
	}

	/**
	 * @brief Reads the request information for reading the base from the JSON input.
	 */
	void InputReaderJson::ReadInputJsonRequestForReadBase() {
		ReadInputJsonSerializeSettings();
		ReadInputJsonStatRequest();
	}

	/**
	 * @brief Updates the stop data in the transport catalogue.
	 * @param tc The transport catalogue to update.
	 */
	void InputReaderJson::UpdStop(TransportCatalogue& tc) {
		for (int i = 0; i < static_cast<int>(update_requests_stop_.size()); ++i) {
			tc.AddStop(update_requests_stop_[i]);
		}
	}

	/**
	 * @brief Updates the stop distances data in the transport catalogue.
	 * @param tc The transport catalogue to update.
	 */
	void InputReaderJson::UpdStopDist(TransportCatalogue& tc) {
		for (int i = 0; i < static_cast<int>(distances_.size()); ++i) {
			tc.AddStopDistance(distances_[i]);
		}
	}

	/**
	 * @brief Updates the bus data in the transport catalogue.
	 * @param tc The transport catalogue to update.
	 */
	void InputReaderJson::UpdBus(TransportCatalogue& tc) {
		for (int i = 0; i < static_cast<int>(update_requests_bus_.size()); ++i) {
			tc.AddBus(update_requests_bus_[i]);
		}
	}

	/**
	 * @brief Returns the render data read from the JSON input.
	 * @return The render data.
	 */
	RenderData InputReaderJson::GetRenderData() {
		return render_data_;
	}

	/**
	 * @brief Updates the route settings in the transport catalogue.
	 * @param tc The transport catalogue to update.
	 */
	void InputReaderJson::UpdRouteSettings(TransportCatalogue& tc) {
		tc.AddRouteSettings(route_settings_);
	}

	/**
	 * @brief Updates the serialization settings in the transport catalogue.
	 * @param tc The transport catalogue to update.
	 */
	void InputReaderJson::UpdSerializeSettings(TransportCatalogue& tc) {
		tc.AddSerializePathToFile(serialize_file_path_);

	}

	/**
	 * @brief Returns the file path for serialization.
	 * @return The file path for serialization.
	 */
	std::string InputReaderJson::GetSerializeFilePath() {
		return serialize_file_path_;
	}
}  // namespace transport_catalogue
