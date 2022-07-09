#pragma once

#include <cmath>
#include <vector>


namespace parsiss
{

struct Point3D
{
    double x, y, z;
    Point3D(double x, double y, double z) : x(x), y(y), z(z) {}
    double distance(const Point3D &other) const
    {
        return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2) + std::pow(z - other.z, 2));
    }
};


struct Point2D
{
    double x, y;
    Point2D(double x, double y) : x(x), y(y) {}
    double distance(const Point2D &other) const
    {
        return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
    }
};

using Points3D = std::vector<Point3D>;
using Points2D = std::vector<Point2D>;

}
