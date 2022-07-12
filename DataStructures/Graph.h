#pragma once 

#include <vector>

namespace parsiss
{
    
struct Vertex
{
    int label;
    Vertex() : label(-1) {}
    Vertex(int label) : label(label) {}

    bool operator==(const Vertex& other) const
    {
        return label == other.label;
    }

    // [[deprecated("only to be used for std::map")]]
    bool operator<(const Vertex& other) const
    {
        return label < other.label;
    }

    struct Iterator { static Vertex end() { return Vertex(-1); } };

};

struct Edge
{
    Vertex from, to;
    int label;

    Edge(Vertex from, Vertex to, int label) : from(from), to(to), label(label) {}
    Edge(Vertex from, Vertex to) : from(from), to(to), label(0) {}

    bool operator ==(const Edge& other) const
    {
        return (from == other.from && to == other.to) || (from == other.to && to == other.from);
    }
};

class Graph
{
public:
    Graph() : Graph(0) {}
    Graph(int vertexCount);
    Graph(int vertexCount, const std::vector<Edge>& edges);
    
    ~Graph();

    bool isEdge(const Edge &edge) const;
    void addEdge(const Edge &edge);
    void removeEdge(const Edge &edge);

    Edge getEdge(const Vertex &from, const Vertex &to) const;
    
    int size() const;
    
    int degree(const Vertex &vertex) const;

    std::vector<Vertex> neighbors(const Vertex &vertex) const;

    void removeVertex(const Vertex &vertex);

    std::vector<Vertex> vertices() const;

    Vertex getNextVertex(const Vertex &vertex) const;
    
public:
    int n;
    std::vector<int> vertex_map;
    std::vector<std::vector<Edge>> adjacency_list;

private:
    int map(const Vertex &vertex) const;
};

}