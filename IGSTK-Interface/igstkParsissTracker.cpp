#include "igstkParsissTracker.h"

#include "igstkParsissTrackerTool.h"
#include "ParsissTool.h"

namespace igstk
{
	Tracker::ResultType ParsissTracker::InternalOpen(void)
	{
		return SUCCESS;
	}

	Tracker::ResultType ParsissTracker::InternalClose(void)
	{
		return SUCCESS;
	}

	Tracker::ResultType ParsissTracker::InternalReset(void)
	{
		return SUCCESS;
	}

	Tracker::ResultType ParsissTracker::InternalStartTracking(void)
	{
		return SUCCESS;
	}

	Tracker::ResultType ParsissTracker::InternalStopTracking(void)
	{
		return SUCCESS;
	}
	
	Tracker::ResultType ParsissTracker::InternalUpdateStatus(void)
	{
		auto tools = camera->getTools();
		TrackerToolsContainerType trackerToolContainer = this->GetTrackerToolContainer();
		
		buffer_lock.lock();
		for (auto it = tools.begin(); it != tools.end(); ++it) {
			auto x = camera->getToolStatus(it->first);
		}
		
		for (const auto tool : trackerToolContainer)
		{
			auto &toolname = tool.first;
			auto& tool_status = camera->getToolStatus(toolname);
			
			TransformType transform;
			transform.SetToIdentity(3000);
			transform.ImportTransform(*(tool_status.transformation));
			this->SetTrackerToolTransformUpdate(tool.second, true);
			this->SetTrackerToolRawTransform(tool.second, transform);
		}
		
		buffer_lock.unlock();
		return SUCCESS;
	}
	
	Tracker::ResultType ParsissTracker::InternalThreadedUpdateStatus(void)
	{
		buffer_lock.lock();
		bool result = camera->update();
		buffer_lock.unlock();
		
		return result ? SUCCESS : FAILURE;
	}

	ParsissTracker::ResultType ParsissTracker::RemoveTrackerToolFromInternalDataContainers(const TrackerToolType* trackerTool)
	{
		camera->removeTool(trackerTool->GetTrackerToolIdentifier());
		return SUCCESS;
	}

	ParsissTracker::ResultType ParsissTracker::VerifyTrackerToolInformation(const TrackerToolType*)
	{
		return SUCCESS;
	}

	ParsissTracker& ParsissTracker::setParsissCamera(parsiss::ParsissCamera* camera)
	{
		this->camera = camera;
		return *this;
	}

	ParsissTracker::ResultType ParsissTracker::AddTrackerToolToInternalDataContainers(const TrackerToolType* trackerTool)
	{
		auto parsiss_tracker_tool = dynamic_cast<const ParsissTrackerTool *> (trackerTool);

		if(!parsiss_tracker_tool) {
			igstkLogMacro(DEBUG, "Tracker tool probably not parsiss tool type ");
			return FAILURE;
		
		}
		
		std::cout << "Adding tool " << parsiss_tracker_tool->GetTrackerToolIdentifier() << std::endl;
		
		auto tracker = new parsiss::ParsissTool(
			parsiss_tracker_tool->GetTrackerToolIdentifier(),
			parsiss_tracker_tool->GetTrackerPatternFile()
		);
		camera->registerTool(tracker);
		return SUCCESS;
	}

}