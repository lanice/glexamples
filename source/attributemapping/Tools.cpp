
#include "Tools.h"

#include <algorithm>
#include <sstream>


std::string Tools::number(size_t num)
{
    std::stringstream stream;
    stream << num;
    return stream.str();
}

std::string Tools::number(int num)
{
    std::stringstream stream;
    stream << num;
    return stream.str();
}

std::string Tools::number(float num)
{
    std::stringstream stream;
    stream << num;
    return stream.str();
}

int Tools::indexOf(const std::vector<std::string> & array, const std::string & value)
{
    int index = std::find(array.begin(), array.end(), value) - array.begin();
    if (index >= 0 && index < (int)array.size()) {
        return index;
    } else {
        return -1;
    }
}
