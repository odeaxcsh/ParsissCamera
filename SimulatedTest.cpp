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

    auto *communication = new ParsissSimulatedCommunication(_("/points.csv"));
    auto camera = ParsissCamera(communication);

    const char *tools_name[] = {
        "Accurect-Pointer",
        "Endonasal-RII",
        "HeadBand-Reference",
        "Navigation-Pointer",
        "Registration-Pointer"
    };

    std::vector<ParsissTool *> tools;
    for (int i = 0; i < 5; i++) {
        tools.push_back(new ParsissTool(
            tools_name[i],
            _(std::string("converted/")) + tools_name[i] + std::string(".txt")
        ));
    }

    for(auto &tool : tools) {
        camera.registerTool(tool);
    }

    std::cout << "Started" << std::endl;
    while(camera.update()) {
        std::cout << "Updated" << std::endl;
        for(auto &tool : tools) {
            std::cout << tool->getName() << ": " << camera.getToolStatus(tool->getName()).visible << std::endl;
        }
        std::cout << "----------------------------------------------------" << std::endl;

        for(auto &tool : tools) {
            if(camera.getToolStatus(tool->getName()).visible) {
                std::cout << tool->getName() << ": " << std::endl;
                auto transformation = camera.getToolStatus(tool->getName()).transformation;
                for(int i = 0; i < 4; i++) {
                    for(int j = 0; j < 4; j++) {
                        std::cout << transformation(i, j) << " ";
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }

    delete communication;
    for(auto &tool : tools) {
        delete tool;
    }
}