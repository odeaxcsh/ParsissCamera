#pragma once

#include "ParsissCommunication.h"
#include "ParsissTool.h"
#include "ParsissToolDetection.h"

#include <map>
#include <string>


namespace parsiss
{

class ParsissCamera
{
public:
    ParsissCamera(std::unique_ptr<ParsissCommunication> communication);
    ~ParsissCamera();

    bool registerTool(std::unique_ptr<const ParsissTool> tool);
    bool removeTool(const std::string &tool_name);

    std::map<std::string, std::unique_ptr<const ParsissTool>> getTools() const;

    bool update();

    ToolStatus getToolStatus(const std::string &tool_name) const;

private:
    std::unique_ptr<ParsissCommunication> communication;
    std::unique_ptr<ToolDetection> td;

    std::map<std::string, ToolStatus> tools_status;

    Frame current_frame;
};

}