#pragma once
#include <er1driver/result.hpp>
#include <memory>



namespace er1driver {

enum class Direction {
    Forward,
    Backward
};

enum class Orientation {
    Left,
    Right
};

class SimpleER1Interface {
public:
    SimpleER1Interface();
    ~SimpleER1Interface();
    SimpleER1Interface(const SimpleER1Interface&) = delete;

    Result connect(std::string);
    void disconnect();

    void setSpeed(float speed);
    Result move(Direction dir);
    Result turn(Orientation ori);
    Result stop();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};


} // namespace er1driver