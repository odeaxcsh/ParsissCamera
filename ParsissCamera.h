#pragma once

#include "ParsissCommunication.h"
#include "ParsissTool.h"
#include "Transformation.h"
#include "ParsissToolDetection.h"

#include <map>
#include <string>


namespace parsiss
{

class ParsissCamera
{
public:
    ParsissCamera(ParsissCommunication *communication);
    ~ParsissCamera();

    ParsissCamera &registerTool(const ParsissTool *tool);
    ParsissCamera &removeTool(const std::string &tool_name);

    bool update();

    ToolStatus getToolStatus(const std::string &tool_name) const;

private:
    ParsissCommunication *communication;
    std::map<std::string, const ParsissTool *> tools;
    std::map<std::string, ToolStatus> tools_status;

    Frame current_frame;
};

}