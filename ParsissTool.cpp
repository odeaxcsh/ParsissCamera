#include "ParsissTool.h"

#include <fstream>
#include <stdexcept>

namespace parsiss
{

ParsissTool::ParsissTool(const std::string &name, const std::string &pattern_file)
{
    this->name = name;
    std::ifstream file(pattern_file);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open pattern file: " + pattern_file);
    }
    
    file >> pattern_points_count;
    for (unsigned int i = 0; i < pattern_points_count; i++) {
        double x, y, z;
        file >> x >> y >> z;
        pattern.push_back(Point3D(x, y, z));
    }
    file.close();
}

ParsissTool::~ParsissTool()
{
}

std::string ParsissTool::getName() const
{
    return name;
}

Pattern ParsissTool::getPattern() const
{
    return pattern;
}

}