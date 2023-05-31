#pragma once

/**
 *@file stat_reader.h
 *@brief This file contains the declaration of the StatReader class, which is responsible for reading and processing statistical queries from the input stream and generating output based on the provided TransportCatalogue.
 */

#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include <deque>

#include "geo.h"
#include "transport_catalogue.h"

namespace transport_catalogue {

	struct OutputRequest {
        std::string name;
        std::string request;
	};

    /**
     * @class StatReader
     *
     * @brief Class responsible for reading and processing statistical queries from the input stream and generating output based on the provided TransportCatalogue.
     */
	class StatReader {
        public:
            /**
             * @brief Constructs a StatReader object with the given input stream.
             *
             * @param is The input stream to read the statistical queries from.
             */
            explicit StatReader(std::istream& is) : input_stream_(is) {};

            /**
             * @brief Gets the number of output queries to be processed.
             *
             * @return The number of output queries.
             */
            int GetNumOutQueries();

            /**
             * @brief Reads and processes the statistical queries from the input stream.
             */
            void FillRequests();

            /**
             * @brief Generates the output based on the provided TransportCatalogue.
             *
             * @param tc The TransportCatalogue object containing the necessary data.
             */
            void Output(std::ostream& os,TransportCatalogue& tc);

        private:
            std::istream& input_stream_;
            std::deque<OutputRequest> requests_;
            int num_update_queries_;                ///< The number of output queries
	};

}// namespace transport_catalogue