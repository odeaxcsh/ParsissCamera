/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>
#include <set>

#include "itkCommand.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkVector.h"
#include "itkVersor.h"

#include "igstkSystemInformation.h"
#include "igstkSerialCommunication.h"
#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"
#include "igstkAuroraTrackerTool.h"
#include "igstkTransform.h"
#include "igstkTransformObserver.h"
 
#include "igstkParsissTrackerTool.h"
#include "igstkParsissTracker.h"

#include "ParsissSimulatedCommunication.h"
#include "ParsissCameraConfiguration.h"
#include "ParsissCamera.h"


int main(int argc, char* argv[])
{	
    std::string data_path = SIMULATED_DATA_ROOT;
	
    using namespace parsiss;
    igstk::RealTimeClock::Initialize();

    typedef igstk::Object::LoggerType     LoggerType;
    typedef itk::StdStreamLogOutput       LogOutputType;
    typedef igstk::TransformObserver      ObserverType;

   
    auto* communication = new ParsissSimulatedCommunication(data_path + "points.csv");
    auto camera = new ParsissCamera(communication);

    const char* tools_name[] = {
        "Accurect-Pointer",
        "Endonasal-RII",
        "HeadBand-Reference",
        "Navigation-Pointer",
        "Registration-Pointer"
    };

    auto tracker = igstk::ParsissTracker::New();
    tracker->setParsissCamera(camera);
	
    std::vector<igstk::ParsissTrackerTool::Pointer> tracker_tools;
    std::vector< ObserverType::Pointer> observers;


    tracker->RequestOpen();

	
    for (int i = 0; i < 5; ++i) {
        igstk::ParsissTrackerTool::Pointer tool = igstk::ParsissTrackerTool::New();
        tracker_tools.push_back(tool);
        tool->SetTrackerToolName(tools_name[i]);
        tool->SetTrackerPatternFile(data_path + std::string("converted/") + tools_name[i] + std::string(".txt"));

        ObserverType::Pointer coordSystemAObserver = ObserverType::New();
        coordSystemAObserver->ObserveTransformEventsFrom(tool);
	    observers.push_back(coordSystemAObserver);
        tool->RequestConfigure();
        tool->RequestAttachToTracker(tracker);
    }

   


    tracker->RequestStartTracking();

    typedef igstk::Transform            TransformType;
    typedef ::itk::Vector<double, 3>    VectorType;
    typedef ::itk::Versor<double>       VersorType;


    for (unsigned int i = 0; i < 1000000; i++)
    {
        igstk::PulseGenerator::CheckTimeouts();

        TransformType             transform;
        VectorType                position;

        for(int i = 0; i < 5; i++)
		{
            auto coordSystemAObserver = observers[i];
			auto trackerTool = tracker_tools[i];
			
            coordSystemAObserver->Clear();
            trackerTool->RequestGetTransformToParent();
            if (coordSystemAObserver->GotTransform())
            {
                transform = coordSystemAObserver->GetTransform();
                if (transform.IsValidNow())
                {
                    position = transform.GetTranslation();
                    std::cout << "Trackertool :"
                        << trackerTool->GetTrackerToolIdentifier()
                        << "\t\t  Position = (" << position[0]
                        << "," << position[1] << "," << position[2]
                        << ")" << std::endl;
                }
            }
		}
    }

    std::cout << "RequestStopTracking()" << std::endl;
    tracker->RequestStopTracking();

    return EXIT_SUCCESS;
}
