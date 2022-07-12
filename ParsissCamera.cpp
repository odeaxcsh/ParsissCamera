#include "ParsissCamera.h"
#include "ParsissToolDetection.h"


namespace parsiss
{

ParsissCamera::ParsissCamera(ParsissCommunication *communication)
{
    this->communication = communication;
}

ParsissCamera::~ParsissCamera()
{
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


bool ParsissCamera::update()
{
    if(communication->connectionClosed()) {
        return false;
    }

    current_frame = communication->getNextFrame();
    tools_status = ToolDetection(tools).detect(current_frame).getToolsStatus();
    return true;
}


ToolStatus ParsissCamera::getToolStatus(const std::string &tool_name) const
{
    return tools_status.at(tool_name);
}

}