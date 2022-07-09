#pragma once

#include "ParsissCommunication.h"

#include <fstream>
#include <string>


namespace parsiss
{

class ParsissSimulatedCommunication : public ParsissCommunication
{
public:
    ParsissSimulatedCommunication(const std::string &simulation_file);
    ~ParsissSimulatedCommunication();

    virtual Frame getNextFrame();
    virtual bool connectionClosed();

private:
    std::ifstream simulation_file;
    std::vector<int> points_index_in_headers;

    virtual Frame readNextFrame();
};

}

