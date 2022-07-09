#pragma once

#include "Matrix.h"
#include "Geometry.h"

namespace parsiss
{
    using Translate = Matrix<3, 1>;
    using Rotate = Matrix<3, 3>;
    using Transformation = Matrix<4, 4>;


    Transformation getTransformation(const Translate &translate, const Rotate &rotate);

    Transformation assessTransformationMatrix(const std::vector<Point3D> &points, const std::vector<Point3D> &points_transformed);
}
