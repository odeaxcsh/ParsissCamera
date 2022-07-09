#pragma once

#include "ParsissTool.h"
#include "ParsissCommunication.h"
#include "Transformation.h"

#include <map>

namespace parsiss
{

struct ToolStatus
{
    bool visible;
    Transformation transformation;
};

using ToolContainer = std::map<std::string, const ParsissTool *>;

class ToolDetection
{
public:
    ToolDetection(const ToolContainer &tools);
    virtual ~ToolDetection();


    virtual std::map<std::string, ToolStatus> getDetectedTools() const;

    virtual ToolDetection &detect(const Frame &frame);


private:

    std::map<std::string, ToolStatus> tools_status;
    ToolContainer tools;


private:
    std::vector<int> findToolInTheFrame(const Frame &frame, const ParsissTool &tool);
};

}
