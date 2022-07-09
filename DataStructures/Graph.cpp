#include "Graph.h"

#include <vector>
#include <algorithm>

#include <stdexcept>

namespace parsiss
{
    
Graph::Graph(int vertexCount)
{
    n = vertexCount;
    vertex_map = std::vector<int>(vertexCount);
    for (int i = 0; i < vertexCount; i++) {
        vertex_map[i] = i;
        adjacency_list.push_back(std::vector<Edge>());
    }
}

Graph::Graph(int vertexCount, const std::vector<Edge>& edges) : Graph(vertexCount)
{
    for (const Edge& edge : edges) {
        addEdge(edge);
    }
}

Graph::~Graph()
{

}

int Graph::map(const Vertex& vertex) const
{
    if (vertex_map[vertex.label] == -1) {
        throw std::runtime_error("Vertex is removed");
    }

    return vertex_map[vertex.label];
}

int Graph::size() const
{
    return n;
}

bool Graph::isEdge(const Edge& edge) const
{
    return std::find(adjacency_list[map(edge.from)].begin(), adjacency_list[map(edge.from)].end(), edge) != adjacency_list[map(edge.from)].end();
}

void Graph::addEdge(const Edge& edge)
{
    if (isEdge(edge) || edge.from == edge.to) {
        return;
    }

    adjacency_list[map(edge.from)].push_back(edge);
    adjacency_list[map(edge.to)].push_back(edge);
}

void Graph::removeEdge(const Edge& edge)
{
    if (!isEdge(edge)) {
        return;
    }

    adjacency_list[map(edge.from)].erase(std::find(adjacency_list[map(edge.from)].begin(), adjacency_list[map(edge.from)].end(), edge));
    adjacency_list[map(edge.to)].erase(std::find(adjacency_list[map(edge.to)].begin(), adjacency_list[map(edge.to)].end(), edge));
}

int Graph::degree(const Vertex& vertex) const
{
    return adjacency_list[map(vertex)].size();
}

std::vector<Vertex> Graph::neighbors(const Vertex& vertex) const
{
    std::vector<Vertex> neighbors;
    for (Edge edge : adjacency_list[map(vertex)]) {
        neighbors.push_back(edge.from == vertex ? edge.to : edge.from);
    }
    return neighbors;
}

std::vector<Vertex> Graph::vertices() const
{
    std::vector<Vertex> vertices;
    for (int i = 0; i < vertex_map.size(); i++) {
        if (vertex_map[i] != -1) {
            vertices.push_back(Vertex(i));
        }
    }
    return vertices;
}

void Graph::removeVertex(const Vertex& vertex)
{
    auto edges = adjacency_list[map(vertex)];
    for (Edge edge : edges) {
        removeEdge(edge);
    }

    adjacency_list.erase(adjacency_list.begin() + map(vertex));
    for (int i = 0; i < vertex_map.size(); i++) {
        if (vertex_map[i] > map(vertex)) {
            vertex_map[i] = vertex_map[i] == -1 ? -1 : vertex_map[i] - 1;
        }
    }
    vertex_map[vertex.label] = -1;
    --n;
}

}