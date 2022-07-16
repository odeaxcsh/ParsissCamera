#pragma once

#include "ParsissTool.h"
#include "ParsissCommunication.h"
#include "Graph.h"


#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"

#include <map>

namespace parsiss
{

struct ToolStatus
{
    bool visible;
    vtkSmartPointer<vtkMatrix4x4> transformation;

    ToolStatus() : visible(false), transformation(nullptr) {}
    ToolStatus(bool visible, vtkMatrix4x4 *transformation) : visible(visible), transformation(transformation) {}
};


class ToolDetection
{
public:
    ToolDetection();
    ~ToolDetection();

    std::map<std::string, ToolStatus> getToolsStatus(const Frame &frames) const;

    bool registerTool(std::unique_ptr<const ParsissTool> tool);
    bool removeTool(const std::string &tool_name);


private:
    std::map<Vertex, Vertex> findPattern(const std::vector<Point3D> &frame, const std::string &tool) const;


private:
    std::map<std::string, std::unique_ptr<const ParsissTool>> tools;
    std::map<std::string, Graph> tools_graphs;
    std::map<std::string, std::vector<double>> weights;
};

}
