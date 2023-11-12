#ifndef X_FOLLOW_COMPONENT_H
#define X_FOLLOW_COMPONENT_H

#include "../Core/defines.h"

struct CFollow
{
    bool bFollow = false;
    std::vector<v2> StringPath;
    v2 TargetPos;
    i32 index = 0;
};

#endif //X_FOLLOW_COMPONENT_H
