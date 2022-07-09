#include "ParsissCamera.h"

namespace parsiss
{

ParsissCamera::ParsissCamera(ParsissCommunication *communication)
{
    this->communication = communication;
}

ParsissCamera::~ParsissCamera()
{
    delete communication;
}

ParsissCamera &ParsissCamera::registerTool(const ParsissTool *tool)
{
    tools[tool->getName()] = tool;
    return *this;
}

ParsissCamera &ParsissCamera::removeTool(const std::string &tool_name)
{
    tools.erase(tool_name);
    return *this;
}


ParsissCamera &ParsissCamera::update()
{
    this->current_frame = communication->getNextFrame();

    // auto td = ToolDetection(current_frame, tools).detect();

    return *this;
}



}