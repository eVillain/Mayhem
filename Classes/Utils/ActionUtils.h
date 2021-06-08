#ifndef ACTION_UTILS_H
#define ACTION_UTILS_H

#include "cocos2d.h"
#include <vector>

namespace ActionUtils {
    void fadeInElements(const std::vector<cocos2d::Node*>& nodes,
                        const float fadeTime,
                        const float interval);

    void fadeOutAndRemove(cocos2d::Node* node,
                          const float delay,
                          const float fadeTime);
};
#endif /* ACTION_UTILS_H */
