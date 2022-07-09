#pragma once

#include "ParsissCommunication.h"
#include "ParsissTool.h"
#include "Transformation.h"

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

    ParsissCamera &update();

    std::vector<ParsissTool *> getVisibleTools() const;
    std::vector<ParsissTool *> getTools() const;

private:
    ParsissCommunication *communication;
    std::map<std::string, const ParsissTool *> tools;
    std::map<std::string, Transformation> tools_transforms;
    std::map<std::string, bool> tools_visiblity;


    Frame current_frame;
};

}