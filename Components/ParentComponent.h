//
// Created by Raul Romero on 2023-10-17.
//

#ifndef X_PARENT_COMPONENT_H
#define X_PARENT_COMPONENT_H

#include <vendor/entt.hpp>

struct CParent
{
    std::vector<entt::entity> Children;
};

#endif //X_PARENT_COMPONENT_H
