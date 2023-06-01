/**
 * @file json_reader.cpp
 * @brief Implementation of the InputReaderJson class for reading input data from JSON format.
 */

#include "transport_catalogue.h"
#include "json.h"
#include "geo.h"
#include "svg.h"
#include "json_reader.h"


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
	svg::Color GetColor(json::Node el) {
		svg::Color color;

		if (el.IsString()) {
			color=el.AsString();
		}
		else {
			if (el.AsArray().size() == 3) {
				Rgb rgb;
				rgb.red = static_cast<uint8_t>(el.AsArray()[0].AsInt());
				rgb.green = static_cast<uint8_t>(el.AsArray()[1].AsInt());
				rgb.blue = static_cast<uint8_t>(el.AsArray()[2].AsInt());
				color = rgb;
			}
			else if (el.AsArray().size() == 4) {
				Rgba rgba;
				rgba.red = static_cast<uint8_t>(el.AsArray()[0].AsInt());
				rgba.green = static_cast<uint8_t>(el.AsArray()[1].AsInt());
				rgba.blue = static_cast<uint8_t>(el.AsArray()[2].AsInt());
				rgba.opacity = el.AsArray()[3].AsDouble();
				color = rgba;
			}
		}


		return color;

	}


	InputReaderJson::InputReaderJson(istream& is) : input_stream_(is) , load_(json::Load(is)){

	}


	void InputReaderJson::ReadInputJsonBaseRequest() {
		const auto& json_array = ((load_.GetRoot()).AsMap()).at("base_requests"s);
		for (const auto& file : json_array.AsArray()) {
			const auto& json_obj = file.AsMap();
			if (json_obj.at("type"s) == "Stop"s) {
				Stop stopjson;
				stopjson.stop_name = json_obj.at("name").AsString();
				stopjson.coordinates.lat = json_obj.at("latitude").AsDouble();
				stopjson.coordinates.lng = json_obj.at("longitude").AsDouble();

				StopDistancesDescription input_stop_dist;
				input_stop_dist.stop_name = json_obj.at("name").AsString();
				auto heighbors = json_obj.at("road_distances");

				for (auto el : heighbors.AsMap()) {
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
				else { 
					bs.type = "false"s; 
				}

				update_requests_bus_.push_back(bs);

			}
		}
	
	}

	void InputReaderJson::ReadInputJsonStatRequest() {
		const auto& json_array_out = ((load_.GetRoot()).AsMap()).at("stat_requests"s);
		for (const auto& file : json_array_out.AsArray()) {
			const auto& json_obj = file.AsMap();
			OutputRequest outputstopjson;
			if (json_obj.at("type").AsString() == "Map"s) {
				outputstopjson.id = json_obj.at("id").AsInt();
				outputstopjson.type = json_obj.at("type").AsString();
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

	void InputReaderJson::ReadInputJsonRenderSettings() {
		const auto& json_array_render = ((load_.GetRoot()).AsMap()).at("render_settings"s);
		if (json_array_render.AsMap().find("width") != json_array_render.AsMap().end()) {
			render_data_.width = json_array_render.AsMap().find("width")->second.AsDouble();
		}

		if (json_array_render.AsMap().find("height") != json_array_render.AsMap().end()) {
			render_data_.height = json_array_render.AsMap().find("height")->second.AsDouble();
		}

		if (json_array_render.AsMap().find("padding") != json_array_render.AsMap().end()) {
			render_data_.padding = json_array_render.AsMap().find("padding")->second.AsDouble();
		}

		if (json_array_render.AsMap().find("line_width") != json_array_render.AsMap().end()) {
			render_data_.line_width = json_array_render.AsMap().find("line_width")->second.AsDouble();
		}

		if (json_array_render.AsMap().find("stop_radius") != json_array_render.AsMap().end()) {
			render_data_.stop_radius = json_array_render.AsMap().find("stop_radius")->second.AsDouble();
		}

		if (json_array_render.AsMap().find("bus_label_font_size") != json_array_render.AsMap().end()) {
			render_data_.bus_label_font_size = json_array_render.AsMap().find("bus_label_font_size")->second.AsDouble();
		}

		if (json_array_render.AsMap().find("bus_label_offset") != json_array_render.AsMap().end()) {

			for (auto el : json_array_render.AsMap().find("bus_label_offset")->second.AsArray()) { 
				render_data_.bus_label_offset.push_back(el.AsDouble()); 
			}
		}

		if (json_array_render.AsMap().find("stop_label_offset") != json_array_render.AsMap().end()) {

			for (auto el : json_array_render.AsMap().find("stop_label_offset")->second.AsArray()) { 
				render_data_.stop_label_offset.push_back(el.AsDouble()); 
			}
		}


		if (json_array_render.AsMap().find("stop_label_font_size") != json_array_render.AsMap().end()) {
			render_data_.stop_label_font_size = json_array_render.AsMap().find("stop_label_font_size")->second.AsDouble();
		}

		if (json_array_render.AsMap().find("underlayer_color") != json_array_render.AsMap().end()) {

			render_data_.underlayer_color.push_back(GetColor(json_array_render.AsMap().find("underlayer_color")->second));
		}

		if (json_array_render.AsMap().find("underlayer_width") != json_array_render.AsMap().end()) {
			render_data_.underlayer_width = json_array_render.AsMap().find("underlayer_width")->second.AsDouble();
		}

		if (json_array_render.AsMap().find("color_palette") != json_array_render.AsMap().end()) {
			for (auto el : json_array_render.AsMap().find("color_palette")->second.AsArray()) {
				render_data_.color_palette.push_back(GetColor(el));
			}

		}

	}


	void InputReaderJson::ReadInputJsonRequest() {
		ReadInputJsonBaseRequest();
		ReadInputJsonStatRequest();
		ReadInputJsonRenderSettings();
	}


	void InputReaderJson::UpdStop(TransportCatalogue& tc) {
		for (int i = 0; i < update_requests_stop_.size(); ++i) {
			tc.AddStop(update_requests_stop_[i]);
		}
	}

	void InputReaderJson::UpdStopDist(TransportCatalogue& tc) {
		for (int i = 0; i < distances_.size(); ++i) {
			tc.AddStopDistance(distances_[i]);
		}
	}

	void InputReaderJson::UpdBus(TransportCatalogue& tc) {
		for (int i = 0; i < update_requests_bus_.size(); ++i) {
			tc.AddBus(update_requests_bus_[i]);
		}
	}

	void InputReaderJson::ManageOutputRequests(TransportCatalogue& tc, MapRenderer& mr) {
		std::ostream& out = std::cout;
		json::Array queries;
		for (auto el : output_requests_) {
			if (el.type == "Bus"s) {

				const Bus* bus_resp = tc.FindBus(el.name);
				if (bus_resp == nullptr) {
					json::Node answer_empty_bus = json::Dict{

							{"error_message"s , "not found"s},
							{"request_id", el.id}
					};
					queries.emplace_back(answer_empty_bus);

				}

				else {
					AllBusInfoBusResponse r = tc.GetAllBusInfo(el.name);
					json::Node non_empty_bus = json::Dict{
							{"curvature", r.route_curvature},
							{"request_id" , el.id},
							{"route_length" , r.route_length},
							{"stop_count" , r.quant_stops},
							{"unique_stop_count" , r.quant_uniq_stops}
					};
					queries.emplace_back(non_empty_bus);
				}
			}

			if (el.type == "Stop"s) {
				const Stop* myStop = tc.FindStop(el.name);
				if (myStop == nullptr) {
					json::Node answer_empty_stop = json::Dict{

							{"error_message"s , "not found"s},
							{"request_id", el.id}
					};
					queries.emplace_back(answer_empty_stop);

				}
				else {
					set<string> r = tc.GetStopInfo(el.name);
					json::Array routes;
					std::copy(r.begin(), r.end(), std::back_inserter(routes));
					json::Node answer_stop = json::Dict{
						{"buses", routes},
						{"request_id" ,  el.id}

					};
					queries.emplace_back(answer_stop);


				}
			}
			if (el.type == "Map"s) {

				string map_str = mr.DrawRouteGetDoc(tc);

				json::Node answer_empty_map = json::Dict{

						{"map"s , map_str},
						{"request_id", el.id}
				};
				queries.emplace_back(answer_empty_map);


			}

		}
		json::Print(json::Document{ queries }, out);
	}

	Render_data InputReaderJson::GetRenderData() {
		return render_data_;
	}

}  // namespace transport_catalogue
