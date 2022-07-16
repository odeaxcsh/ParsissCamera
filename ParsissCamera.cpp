#include "ParsissCamera.h"
#include "ParsissToolDetection.h"


namespace parsiss
{

ParsissCamera::ParsissCamera(std::unique_ptr<ParsissCommunication> communication)
{
    this->communication = std::move(communication);
    this->td = std::make_unique<ToolDetection>();
}

ParsissCamera::~ParsissCamera()
{
}

bool ParsissCamera::registerTool(std::unique_ptr<const ParsissTool> tool)
{
    return td->registerTool(std::move(tool));
}

bool ParsissCamera::removeTool(const std::string &tool_name)
{
    return td->removeTool(tool_name);

}


bool ParsissCamera::update()
{
    if(communication->connectionClosed()) {
        return false;
    }

    current_frame = communication->getNextFrame();
    tools_status = td->getToolsStatus(current_frame);
    return true;
}


ToolStatus ParsissCamera::getToolStatus(const std::string &tool_name) const
{
    return tools_status.at(tool_name);
}

}