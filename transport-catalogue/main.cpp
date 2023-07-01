#include "geo.h"
#include "transport_catalogue.h"
#include "json_reader.h"
#include "iostream"
#include "map_renderer.h"
#include "request_handler.h"
#include <fstream>
using namespace transport_catalogue;
using namespace std;
#include <chrono>
#include "serialization.h"
#include "transport_router.h"
#include <string_view>

using namespace std::literals;
void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {

        transport_catalogue::TransportCatalogue tc;
        transport_catalogue::InputReaderJson reader(std::cin);
        (void)reader.ReadInputJsonRequestForFillBase();

        reader.UpdStop(tc);
        reader.UpdBus(tc);
        reader.UpdStopDist(tc);
        reader.UpdRouteSettings(tc);
        reader.UpdSerializeSettings(tc);

        RenderData rd = reader.GetRenderData();

        ofstream out_file(tc.GetSerializerFilePath(), ios::binary);

        domain::RouteSettings routeSettings = tc.GetRouteSettings();

        serialization::catalogue_serialization(tc, rd , routeSettings, out_file);

    }
    else if (mode == "process_requests"sv) {

        transport_catalogue::InputReaderJson reader(std::cin);
        (void)reader.ReadInputJsonRequestForReadBase();

        ifstream in_file(reader.GetSerializeFilePath(), ios::binary);
        auto catalogue = serialization::catalogue_deserialization(in_file);
        RenderData rd = catalogue.render_settings_;
        transport_catalogue::TransportCatalogue tc = catalogue.transport_catalogue_;
        tc.AddRouteSettings(catalogue.routing_settings_);

        MapRenderer mapdrawer(rd);
        graph::TransportRouter transport_router(tc);
        reader.ManageOutputRequests(tc, mapdrawer, transport_router);
    }
    else {
        PrintUsage();
        return 1;
    }
}
