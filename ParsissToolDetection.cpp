#include "ParsissToolDetection.h"

#include "Graph.h"
#include "Heap.hpp"


#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkLandmarkTransform.h"
#include "vtkIterativeClosestPointTransform.h"
#include "vtkMatrix4x4.h"

#include <vector>
#include <functional>


namespace parsiss
{

ToolDetection::ToolDetection(const ToolContainer &tools)
{
    this->tools = tools;
    for (auto &tool : tools) {
        tools_status[tool.first] = {false, Transformation()};
    }
}


ToolDetection::~ToolDetection()
{
}


std::map<std::string, ToolStatus> ToolDetection::getDetectedTools() const
{
    std::map<std::string, ToolStatus> detected_tools;
    for (auto &tool : tools_status) {
        detected_tools[tool.first] = tool.second;
    }
    return detected_tools;
}


Transformation registerPointsTransformation(const Points3D &from, const Points3D &to)
{
    vtkPoints *from_vtk = vtkPoints::New();
    vtkCellArray *from_vtk_cells = vtkCellArray::New();
    for (auto &point : from) {
        auto id = from_vtk->InsertNextPoint(point.x, point.y, point.z);
        from_vtk_cells->InsertNextCell(1);
        from_vtk_cells->InsertCellPoint(id);
    }
    vtkPolyData *from_vtk_polydata = vtkPolyData::New();
    from_vtk_polydata->SetPoints(from_vtk);
	from_vtk_polydata->SetVerts(from_vtk_cells);


    vtkPoints *to_vtk = vtkPoints::New();
    vtkCellArray *to_vtk_cells = vtkCellArray::New();
    for (auto &point : to) {
        auto id = to_vtk->InsertNextPoint(point.x, point.y, point.z);
        to_vtk_cells->InsertNextCell(1);
        to_vtk_cells->InsertCellPoint(id);
    }
    vtkPolyData *to_vtk_polydata = vtkPolyData::New();
    to_vtk_polydata->SetPoints(to_vtk);
	to_vtk_polydata->SetVerts(to_vtk_cells);

    
    vtkIterativeClosestPointTransform *icp = vtkIterativeClosestPointTransform::New();
    icp->SetSource(from_vtk_polydata);
    icp->SetTarget(to_vtk_polydata);
    icp->SetStartByMatchingCentroids(true);
    icp->SetMaximumNumberOfIterations(100);
    icp->GetLandmarkTransform()->SetModeToRigidBody();
    icp->Update();

    vtkLandmarkTransform *landmark_transform = icp->GetLandmarkTransform();
    vtkMatrix4x4 *matrix = landmark_transform->GetMatrix();
    Transformation transformation;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            transformation(i, j) = matrix->GetElement(i, j);
        }
    }
    return transformation;
}


using Pattern = std::vector<Point3D>;
using Clique = std::vector<Vertex>;

template <typename dtype>
std::vector<Edge> distances(const std::vector<dtype>& data)
{
    std::vector<Edge> result;
    for (int i = 0; i < data.size(); i++) {
        for (int j = i + 1; j < data.size(); j++) {
            result.push_back(Edge(i, j, data[i].distance(data[j])));
        }
    }
    return result;
}

template <typename dtype, typename function>
std::vector<dtype> filter(const std::vector<dtype>& data, function predicate)
{
    std::vector<dtype> result;
    for (auto& item : data) {
        if (predicate(item)) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<Edge> findPatternEdges(const std::vector<Edge>& edges, const std::vector<double>& weights, double precision)
{
    std::vector<Edge> result;
    for (auto& edge : edges) {
        auto it = std::find_if(weights.begin(), weights.end(), [&edge, precision](double w) {
            return std::abs(w - edge.weight) < precision;
            });
        if (it != weights.end()) {
            result.push_back(edge);
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



Clique findCliqueOfSize3(const Graph& graph)
{
    for(auto v : graph.vertices()) {
        for(auto u : graph.neighbors(v)) {
            for(auto w : graph.neighbors(u)) {
                if (graph.isEdge({ v, w }) && graph.isEdge({ u, w }) && graph.isEdge({ v, u })) {
                    return { v, u, w };
                }
            }
        }
    }
    
    return {};
}

Clique findCliqueOfSize4(const Graph& graph)
{
    for (auto v : graph.vertices()) {
        for (auto u : graph.neighbors(v)) {
            for (auto w : graph.neighbors(u)) {
                for (auto x : graph.neighbors(w)) {
                    if (graph.isEdge({ v, w }) && graph.isEdge({ u, w }) && graph.isEdge({ v, u }) && graph.isEdge({ w, x }) && graph.isEdge({ u, x }) && graph.isEdge({ v, x })) {
                        return { v, u, w, x };
                    }
                }
            }
        }
    }
    
    return {};
}


std::vector<Clique> findCliques(Graph& graph, int size)
{
    std::function <Clique(const Graph&)> fc;
    if (size == 3) {
        fc = findCliqueOfSize3;
    }
    else if (size == 4) {
        fc = findCliqueOfSize4;
    }
    else {
        throw std::runtime_error("Unsupported clique size");
    }

    std::vector<Clique> result;
    for (int i = 0; i < graph.n; i++) {
        auto clique = fc(graph);
        if (clique.empty()) {
            continue;
        }
        result.push_back(clique);
        for (auto& v : clique) {
            graph.removeVertex(v);
        }
    }

    return result;
}

std::vector<int> ToolDetection::findToolInTheFrame(const Frame &frame, const ParsissTool &tool)
{
    auto pattern = tool.getPattern();
    auto weights = [](auto data){
        auto result = std::vector<double>();
        auto edges = distances(data);
        for (auto& edge : edges) {
            result.push_back(edge.weight);
        }
        return result;
    }(pattern);

    Graph g = Graph(
        frame.size(),
        findPatternEdges(distances(frame), weights, 2)
    );

    g = eliminateVertices(g, pattern.size() - 1);
    auto cliques = findCliques(g, pattern.size());
    if(cliques.size() == 0) {
        return std::vector<int>();
    } 
    if(cliques.size() > 1) {
        throw std::runtime_error("More than one tool of a specific type in the scene found");
    }
    
    auto clique = cliques[0];
    auto indices = std::vector<int>();
    for (auto vertex : clique) {
        indices.push_back(vertex.label);
    }
    return indices;
}


ToolDetection &ToolDetection::detect(const Frame &frame)
{
    for (auto &[name, tool] : tools) {
        auto tool_points_indices = findToolInTheFrame(frame, *tool);
    
        if (tool_points_indices.size() > 0) {
            Points3D tool_points;
            for (auto index : tool_points_indices) {
                tool_points.push_back(frame[index]);
            }
			
            tools_status[name] = ToolStatus({true, registerPointsTransformation(tool->getPattern(), tool_points)});
        } else {
            tools_status[name] = {false, Transformation()};
        }
    }
    return *this;
}


}