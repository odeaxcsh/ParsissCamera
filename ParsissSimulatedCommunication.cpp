#include "ParsissSimulatedCommunication.h"
#include "StringUtils.h"

#include <stdexcept>

namespace parsiss
{

ParsissSimulatedCommunication::ParsissSimulatedCommunication(const std::string &simulation_file_path)
{
    simulation_file = std::ifstream(simulation_file_path);
    if (!simulation_file) {
        throw std::runtime_error("Could not open simulation file");
    }

    std::string line;
    std::getline(simulation_file, line);
    auto file_headers = split(line, ",");

    for(int i = 0; i < file_headers.size(); i++) {
        if (file_headers[i] == "Tx") {
            points_index_in_headers.push_back(i);
        }
    }

    if (points_index_in_headers.size() == 0) {
        throw std::runtime_error("Could not find any point in simulation file");
    }
}

ParsissSimulatedCommunication::~ParsissSimulatedCommunication()
{
    simulation_file.close();
}


bool ParsissSimulatedCommunication::connectionClosed()
{
    return !simulation_file.good();
}


Frame ParsissSimulatedCommunication::getNextFrame()
{
    return readNextFrame();
}


Frame ParsissSimulatedCommunication::readNextFrame()
{
    Frame frame;
    std::string line;
    std::getline(simulation_file, line);
    auto row_elements = split(line, ",");
    for(auto index : points_index_in_headers) {
        
        frame.push_back(Point3D(std::stod(row_elements[index]), std::stod(row_elements[index + 1]), std::stod(row_elements[index + 2])));
    }
    return frame;
}

}
