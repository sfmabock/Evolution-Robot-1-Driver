#include <er1driver/result.hpp>

namespace er1driver {

Result::Result(ResultCode code) : code_(code) {}

Result::Result(std::string msg) : msg_(std::move(msg)) {}

Result::Result(ResultCode code, std::string msg) : code_(code), msg_(std::move(msg)) {}

Result::Result(const char *msg) : msg_(msg) {}

void Result::ingore() {};

ResultCode Result::code() const {
    return code_;
}

const std::string &Result::msg() const {
    return msg_;
}

bool Result::hasMsg() const {
    return !msg_.empty();
}

Result::operator bool() const {
    return code_ == ResultCode::Success;
}

} // namespace er1driver