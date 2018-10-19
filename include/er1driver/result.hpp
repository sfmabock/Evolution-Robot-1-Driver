#pragma once

#include <string>

#define RETURN_ON_ERROR(x) { const er1driver::Result& result_##__LINE__ = (x); if (!result_##__LINE__) return result_##__LINE__;}


namespace er1driver {

enum class ResultCode {
    Success = 0,
    Failure = 1,
};

class Result {
public:
    Result() = default;
    Result(const Result&) = default;
    Result(ResultCode code);
    Result(std::string msg);
    Result(ResultCode code, std::string msg);
    Result(const char* msg);

    template<class T>
    Result(const T& value);

    void ingore();
    ResultCode code() const;
    const std::string& msg() const;
    bool hasMsg() const;
    operator bool() const;


private:
    ResultCode code_ = ResultCode::Failure;
    std::string msg_;
};

template<class T>
Result::Result(const T& value) {
    if (static_cast<bool>(value)) {
        code_ = ResultCode::Success;
    } else {
        code_ = ResultCode::Failure;
    }
}


} // namespace er1driver


