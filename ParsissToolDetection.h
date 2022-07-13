#pragma once

#include "ParsissTool.h"
#include "ParsissCommunication.h"
#include "Graph.h"


#include "vtkMatrix4x4.h"


#include <map>

namespace parsiss
{

struct ToolStatus
{
    bool visible;
    vtkMatrix4x4 *transformation;

    ToolStatus() : visible(false), transformation(nullptr) {}
    ToolStatus(bool visible, vtkMatrix4x4 *transformation) : visible(visible), transformation(transformation) {}
};

using ToolContainer = std::map<std::string, const ParsissTool *>;

class ToolDetection
{
public:
    ToolDetection(const ToolContainer &tools);
    virtual ~ToolDetection();


    virtual std::map<std::string, ToolStatus> getToolsStatus() const;

    virtual ToolDetection &detect(const Frame &frame);


private:
    std::map<Vertex, Vertex> findPattern(const std::vector<Point3D> &frame, const std::string &tool);


private:
    std::map<std::string, ToolStatus> tools_status;
    ToolContainer tools;
    std::map<std::string, Graph> tools_graphs;
    std::map<std::string, std::vector<double>> weights;
};

}
