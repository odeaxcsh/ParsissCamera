#include "ParsissCamera.h"
#include "ParsissSimulatedCommunication.h"

#include "ParsissCameraConfiguration.h"

#include <iostream>


std::string translate_path(std::string path)
{
    return std::string(SIMULATED_DATA_ROOT) + "/" + path;
}


int main()
{
    using namespace parsiss;

    auto _ = translate_path;

    std::unique_ptr<ParsissCommunication> communication(new ParsissSimulatedCommunication(_("/points.csv")));
    std::unique_ptr<ParsissCamera> camera(new ParsissCamera(std::move(communication)));

    const char *tools_name[] = {
        "Accurect-Pointer",
        "Endonasal-RII",
        "HeadBand-Reference",
        "Navigation-Pointer",
        "Registration-Pointer"
    };

    std::vector<std::unique_ptr<const ParsissTool>> tools;
    for (int i = 0; i < 5; i++) {
        tools.push_back(std::make_unique<ParsissTool>(tools_name[i], _(std::string("converted/")) + tools_name[i] + std::string(".txt")));
    }

    for(auto &tool : tools) {
        camera->registerTool(std::move(tool));
    }

    std::cout << "Started" << std::endl;
    while(camera->update()) {
        std::cout << "Updated" << std::endl;
        for(int i = 0; i < 5; ++i) {
            std::cout << tools_name[i] << ": " << camera->getToolStatus(tools_name[i]).visible << std::endl;
        }
        std::cout << "----------------------------------------------------" << std::endl;

        for(int i = 0; i < 5; ++i) {
            if(camera->getToolStatus(tools_name[i]).visible) {
                std::cout << tools_name[i] << ": " << std::endl;
                auto transformation = camera->getToolStatus(tools_name[i]).transformation;
                for(int i = 0; i < 4; i++) {
                    for(int j = 0; j < 4; j++) {
                        std::cout << transformation->GetElement(i, j) << " ";
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }


}