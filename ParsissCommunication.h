#pragma once

#include <vector>

#include "Geometry.h"

namespace parsiss
{

using Frame = Points3D;

class ParsissCommunication
{
public:
    virtual ~ParsissCommunication() = default;
    virtual bool connectionClosed() = 0;
    virtual Frame getNextFrame() = 0;
};

}