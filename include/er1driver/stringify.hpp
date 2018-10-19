#pragma once

#include <string>
#include <sstream>

namespace er1driver {
template<class T>
std::string strcatHelper(std::stringstream &ss, const T &val) {
    ss << val;
    return ss.str();
}

template<class T, class... Args>
std::string strcatHelper(std::stringstream &ss, const T &val, Args... args) {
    ss << std::fixed << val;
    return strcatHelper(ss, args...);
}

template<class... Args>
std::string stringify(Args... args) {
    std::stringstream ss;
    return strcatHelper(ss, args...);
}

} // namespace er1driver


