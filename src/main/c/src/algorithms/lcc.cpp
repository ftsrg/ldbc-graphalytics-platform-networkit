#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>
#include <chrono>

#include <networkit/auxiliary/Parallelism.hpp>
#include <networkit/io/EdgeListReader.hpp>
#include <networkit/centrality/LocalClusteringCoefficient.hpp>

#include "utils.h"

using LCC_Result = std::vector<std::pair<NetworKit::node, double>>;
using Graph_Mapping = std::map<std::string, NetworKit::node>;

/*
 * Result serializer function
 */
void WriteOutLCCResult(
    const LCC_Result &result,
    const Graph_Mapping &mapping,
    const BenchmarkParameters &parameters
) {
    std::ofstream file{parameters.output_file};
    if (!file.is_open()) {
        std::cerr << "File " << parameters.output_file << " does not exists" << std::endl;
        exit(-1);
    }
    file.precision(16);
    file << std::scientific;

    auto reverseMapping = ReverseMap(mapping);
    for (const auto &rank : result) {
        std::string original_index = reverseMapping[rank.first];
        file << original_index << " " << rank.second << std::endl;
    }
}

int main(int argc, char **argv) {
    BenchmarkParameters parameters = ParseBenchmarkParameters(argc, argv);

    // Set used number of threads
    Aux::setNumberOfThreads(parameters.thread_num);

    // Read the graph
    auto reader = NetworKit::EdgeListReader(
        ' ',
        0,
        "#",
        false,
        parameters.directed
    );
    NetworKit::Graph graph = reader.read(parameters.input_dir + "/edge.csv");

    // Execute the algorithm
    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    NetworKit::LocalClusteringCoefficient lcc(graph, true);
    lcc.run();
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    // Write out the results
    WriteOutLCCResult(
        lcc.ranking(),
        reader.getNodeMap(),
        parameters
    );
}
