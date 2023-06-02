#define _CRT_SECURE_NO_WARNINGS
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


int main() {
#ifdef _DEBUG
	if (freopen("input.txt.txt", "r", stdin) == nullptr) {
		puts("can't open input.txt.txt");
		return 1;
	}
#endif 
	
	
	transport_catalogue::TransportCatalogue tc;
	transport_catalogue::InputReaderJson reader(std::cin);
	(void)reader.ReadInputJsonRequest();

	reader.UpdStop(tc);
	reader.UpdBus(tc);
	reader.UpdStopDist(tc);
	RenderData rd = reader.GetRenderData();
	MapRenderer mapdrawer(rd);
	reader.ManageOutputRequests(tc, mapdrawer);
	
	
	
	/*
	transport_catalogue::TransportCatalogue tc;
	transport_catalogue::InputReaderJson reader(std::cin);
	(void)reader.ReadInputJsonRequest();
	reader.UpdStop(tc);
	reader.UpdBus(tc);
	reader.UpdStopDist(tc);
	RenderData rd = reader.GetRenderData();
	MapRenderer mapdrawer(rd);
	RequestHandler reqhandleq(tc, mapdrawer);
	//reqhandleq.RenderMap();
	reqhandleq.RenderMapByString();
	
	*/

	return 0;


}