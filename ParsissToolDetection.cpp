#include "ParsissToolDetection.h"

#include "Graph.h"
#include "Heap.hpp"

#include "vtkMatrix4x4.h"
#include "vtkPoints.h"
#include "vtkLandmarkTransform.h"

#include <vector>
#include <functional>
#include <stdexcept>


namespace parsiss
{

ToolDetection::ToolDetection(const ToolContainer &tools)
{
    this->tools = tools;
    for (auto &tool : tools) {
        tools_status[tool.first] = {false, Transformation()};
    }

    for(auto [name, tool] : tools) {
        const auto &pattern = tool->getPattern();
        int n = pattern.size();
        tools_graphs[name] = Graph(n);
        for(int i = 0; i < n; i++) {
            for(int j = i + 1; j < n; j++) {
                weights[name].push_back(pattern[i].distance(pattern[j]));
                tools_graphs[name].addEdge({i, j, static_cast<int> (weights[name].size() - 1)});
            }
        }
    }
}


ToolDetection::~ToolDetection()
{
}


std::map<std::string, ToolStatus> ToolDetection::getToolsStatus() const
{
    return tools_status;
}


Transformation convertVTKTransformation(vtkLandmarkTransform *transform)
{
    Transformation transformation;
    auto matrix = transform->GetMatrix();
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            transformation(i, j) = matrix->GetElement(i, j);
        }
    }
    return transformation;
}


Transformation registerPointsTransformation(const Points3D &from, const Points3D &to)
{
    if(from.size() != to.size()) {
        throw std::runtime_error("Points size mismatch");
    }

    vtkPoints *from_vtk = vtkPoints::New();
    vtkPoints *to_vtk = vtkPoints::New();
    for(int i = 0; i < from.size(); i++) {
        from_vtk->InsertNextPoint(from[i].x, from[i].y, from[i].z);
        to_vtk->InsertNextPoint(to[i].x, to[i].y, to[i].z);
    }

    auto *registration = vtkLandmarkTransform::New();
    registration->SetSourceLandmarks(from_vtk);
    registration->SetTargetLandmarks(to_vtk);
    registration->SetModeToRigidBody();
    registration->Modified();
    registration->Update();

    return convertVTKTransformation(registration);
}

#include <iostream>

std::vector<Edge> labelDistances(const std::vector<Point3D> &points, std::vector<double> &weights, double precision)
{
    std::vector<Edge> result;
    for(int i = 0; i < points.size(); i++) {
        for(int j = i + 1; j < points.size(); j++) {
            double distance = points[i].distance(points[j]);			
            auto it = std::find_if(weights.begin(), weights.end(), [&distance, precision](double w) {
			
                return std::abs(w - distance) <= precision;
            });

            if (it != weights.end()) {
                result.push_back({i, j, static_cast<int> (it - weights.begin())});
            }
        }
    }

    return result;
}


Graph& eliminateVertices(Graph& graph, int lowerBound)
{
    auto heap = MinHeap<Vertex, int>();
    for (int i = 0; i < graph.n; i++) {
        heap.push(i, graph.degree(i));
    }

    while (!heap.empty() && heap.top().second < lowerBound) {
        auto [v, _] = heap.pop();
        for (auto& u : graph.neighbors(v)) {
            heap.update(u, graph.degree(u) - 1);
        }
        graph.removeVertex(v);
    }

    return graph;
}


bool corresponds(const Graph &g, const Graph &h, std::map<Vertex, Vertex> mapping, Vertex v)
{
    for (auto& [key, value] : mapping) {
        if(g.getEdge(key, v).label != h.getEdge(value, mapping[v]).label) {
            auto e1 = g.getEdge(key, v);
			auto e2 = h.getEdge(value, mapping[v]);
            return false;
        }
    }

    return true;
}


std::map<Vertex, Vertex> findSubgraphs(const Graph &g, const Graph &h)
{
    std::vector<
        std::map<Vertex, Vertex>
    > mappings = { {} };

    while(!mappings.empty()) {
        auto mapping = mappings.back();
        mappings.pop_back();

        if(mapping.size() == h.n) {
            return mapping;
        }

        for(auto v : g.vertices()) {
            if(mapping.find(v) == mapping.end()) {
                auto level = mapping.size();
                std::map<Vertex, Vertex> new_mapping = mapping;
                new_mapping[v] = Vertex(level);
                if(corresponds(g, h, new_mapping, v)) {
                    mappings.push_back(new_mapping);
                }
            }
        }
    }
    return {};
}


std::map<Vertex, Vertex> ToolDetection::findPattern(const std::vector<Point3D> &frame, const std::string &tool)
{
    auto edges = labelDistances(frame, weights[tool], 1);
    auto g = Graph(frame.size(), edges);
    eliminateVertices(g, tools_graphs[tool].size() - 1);
    auto mapping = findSubgraphs(g, tools_graphs[tool]);
    return mapping;
}


ToolDetection &ToolDetection::detect(const Frame &frame)
{
    for (auto &[name, tool] : tools) {
        auto mapping = findPattern(frame, name);
    
        if (mapping.size() > 0) {
            Points3D tool_points(tools[name]->getPattern().size());
            for(auto [key, value] : mapping) {
                tool_points[value.label] = frame[key.label];
            }

            tools_status[name] = ToolStatus({true, registerPointsTransformation(tool->getPattern(), tool_points)});
        } else {
            tools_status[name] = {false, Transformation()};
        }
    }
    return *this;
}


}