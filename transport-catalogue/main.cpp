#include "stat_reader.h"
#include "geo.h"
#include "transport_catalogue.h"
#include "input_reader.h"

using namespace std;

int main() {
    transport_catalogue::TransportCatalogue tc;
    transport_catalogue::InputReader input_reader(std::cin);
    (void)input_reader.GetNumUpdateQueries();
    input_reader.FillRequests();

    input_reader.UpdStop(tc);
    input_reader.UpdBus(tc);
    input_reader.UpdStopDist(tc);

    transport_catalogue::StatReader stat_reader(std::cin);
    (void)stat_reader.GetNumOutQueries();
    stat_reader.FillRequests();
    stat_reader.Output(tc);

    return 0;
}
