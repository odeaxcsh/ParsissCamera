#pragma once

#include "Geometry.h"

#include <string>
#include <vector>

namespace parsiss
{

using Pattern = Points3D;

class ParsissTool
{
public:
    ParsissTool(const std::string &name, const std::string &pattern_file);
    ~ParsissTool();

    std::string getName() const;
    Pattern getPattern() const;

private:
    std::string name;
    unsigned int pattern_points_count;
    Pattern pattern;
};

}