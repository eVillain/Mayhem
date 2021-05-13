#ifndef NUMBER_FORMATTER_H
#define NUMBER_FORMATTER_H

#include <iomanip>
#include <sstream>

namespace NumberFormatter {

    static std::string toString(const float value,
                                const int precision = 2)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}
};
#endif /* NUMBER_FORMATTER_H */
