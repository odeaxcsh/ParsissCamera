#include "igstkParsissTrackerTool.h"



namespace igstk
{

void ParsissTrackerTool::SetTrackerToolName(const std::string& name)
{
	this->SetTrackerToolIdentifier(name);
}

void ParsissTrackerTool::SetTrackerPatternFile(const std::string& path)
{
	patternFilePath = path;
}

std::string ParsissTrackerTool::GetTrackerPatternFile() const
{
	return patternFilePath;
}

bool ParsissTrackerTool::CheckIfTrackerToolIsConfigured() const
{
	return true;
}

ParsissTrackerTool::ParsissTrackerTool()
	:m_StateMachine(this)
{
}

}