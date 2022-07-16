#pragma once


#include "igstkTracker.h"
#include "ParsissCamera.h"


#include <mutex>

namespace igstk
{

class ParsissTracker : public Tracker
{
public:
	igstkStandardClassTraitsMacro(ParsissTracker, Tracker)

	ParsissTracker &setParsissCamera(std::unique_ptr<parsiss::ParsissCamera> camera);

protected:
	/** Open communication with the tracking device. */
	virtual ResultType InternalOpen(void) override;

	/** Close communication with the tracking device. */
	virtual ResultType InternalClose(void) override;

	/** Reset the tracking device to put it back to its original state. */
	virtual ResultType InternalReset(void) override;

	/** Put the tracking device into tracking mode. */
	virtual ResultType InternalStartTracking(void) override;

	/** Take the tracking device out of tracking mode. */
	virtual ResultType InternalStopTracking(void) override;

	/** Update the status and the transforms for all TrackerTools. */
	virtual ResultType InternalUpdateStatus(void) override;

	/** Update the status and the transforms. This function is called by a separate thread. */
	virtual ResultType InternalThreadedUpdateStatus(void) override;


	virtual ResultType VerifyTrackerToolInformation(const TrackerToolType*) override;


	/** This method will remove entries of the traceker tool from internal
	  * data containers */
	virtual ResultType RemoveTrackerToolFromInternalDataContainers(const TrackerToolType* trackerTool) override;

	/** Add tracker tool entry to internal containers */
	virtual ResultType AddTrackerToolToInternalDataContainers(const TrackerToolType* trackerTool) override;
	
private:
	ParsissTracker() : m_StateMachine(this) {}
	std::unique_ptr<parsiss::ParsissCamera> camera;
	std::mutex buffer_lock;
};
	
}
