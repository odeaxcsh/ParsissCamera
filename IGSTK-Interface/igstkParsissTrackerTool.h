#pragma once

#include "igstkTrackerTool.h"
#include "ParsissTool.h"

#include <string>


namespace igstk
{

class ParsissTrackerTool : public TrackerTool
{
public:
	/** Macro with standard traits declarations. */
	igstkStandardClassTraitsMacro(ParsissTrackerTool, TrackerTool)


	void SetTrackerToolName(const std::string& name);
	void SetTrackerPatternFile(const std::string& path);

	std::string GetTrackerPatternFile() const;

	virtual bool CheckIfTrackerToolIsConfigured() const override;

private:
	ParsissTrackerTool();
	std::string patternFilePath;
};
	
}
