#ifndef ACTION_UTILS_H
#define ACTION_UTILS_H

#include "cocos2d.h"
#include <vector>

namespace ActionUtils {
    void fadeInElements(const std::vector<cocos2d::Node*>& nodes,
                        const float fadeTime,
                        const float interval);
};
#endif /* ACTION_UTILS_H */
