#include "Transformation.h"
#include "Matrix.hpp"

#include <stdexcept>


namespace parsiss
{

Transformation getTransformation(const Translate &translate, const Rotate &rotate)
{
    return Transformation(std::vector<double>({
            rotate(0, 0), rotate(0, 1), rotate(0, 2), translate(0),
            rotate(1, 0), rotate(1, 1), rotate(1, 2), translate(1),
            rotate(2, 0), rotate(2, 1), rotate(2, 2), translate(2),
            0, 0, 0, 1
    }));
}


Transformation assessTransformationMatrix(const std::vector<Point3D> &points, const std::vector<Point3D> &points_transformed)
{
    if (points.size() != points_transformed.size()) {
        throw std::runtime_error("Points size mismatch");
    }

    if (points.size() < 3) {
            throw std::runtime_error("patternPoints and dataPoints must have at least 4 points");
    }

    
    // TODO: Here we use only the first three points to calculate the transformation matrix.
    // *      This is not the best way to do it, but it is enough for now.
    // *      In the future, for more accurate results, we should use the whole set of points.
    // *      A simple way to do so is to use Linear Regression to solved the linear equations.

    Transformation transformation;
    Matrix<4, 4> A = std::vector<double>({
            points[0].x, points[0].y, points[0].z, 1,
            points[1].x, points[1].y, points[1].z, 1,
            points[2].x, points[2].y, points[2].z, 1,
            0, 0, 0, 1
    });

    auto [L, U, P] = A.lu(A);

    for (int i = 0; i < 3; i++) {
        auto point = points_transformed[i];
        std::vector<double> b = { point.x, point.y, point.z, 1 };

        auto x = Matrix<4, 4>::solve(L, U, P, b);
        for (int j = 0; j < 4; j++) {
            transformation(i, j) = x[j];
        }
    }
    transformation(3, 0) = 0;
    transformation(3, 1) = 0;
    transformation(3, 2) = 0;
    transformation(3, 3) = 1;

    return transformation;
}

}