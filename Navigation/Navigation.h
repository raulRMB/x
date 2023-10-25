//
// Created by Raul Romero on 2023-10-25.
//

#ifndef X_NAVIGATION_H
#define X_NAVIGATION_H

#include "base/defines.h"
#include <glm/gtx/norm.hpp>
#include <vector>
#include <util/primitives.h>

namespace x::Navigation
{

// Function to find the circumcenter and circumradius of a triangle
void FindCircumcircle(const Triangle2D& triangle, glm::vec2& circumcenter, float& circumradius);
std::vector<Triangle2D> BowyerWatson(std::vector<v2>& points);

}


#endif //X_NAVIGATION_H
