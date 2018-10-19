#include <er1driver/er1driver.hpp>
#include <er1driver/controlmodule.hpp>
#include <iostream>


namespace er1driver {


class SimpleER1Interface::Impl {
public:
    Impl() = default;
    ~Impl() = default;

    Result connect(std::string);

    Result init(Motor motor);

    void disconnect();

    void setSpeed(float speed);
    Result move(Direction dir);
    Result turn(Orientation ori);
    Result stop();

private:
    ControlModule module_;
    int speed_ = 200000;
};


SimpleER1Interface::SimpleER1Interface() {
    impl_ = std::make_unique<Impl>();
}

SimpleER1Interface::~SimpleER1Interface() {

}

Result SimpleER1Interface::Impl::connect(std::string name) {
    RETURN_ON_ERROR(module_.connect(std::move(name)));
    RETURN_ON_ERROR(init(Motor::One));
    RETURN_ON_ERROR(init(Motor::Two));
    return ResultCode::Success;
}

Result SimpleER1Interface::Impl::init(er1driver::Motor motor) {
    RETURN_ON_ERROR(module_.reset(motor));
    RETURN_ON_ERROR(module_.setLimitSwitchMode(motor, LimitSwitchMode::Off));
    RETURN_ON_ERROR(module_.setMotorMode(motor, MotorMode::On));
    RETURN_ON_ERROR(module_.setProfileMode(motor, ProfileMode::VelocityContouring));
    RETURN_ON_ERROR(module_.setMotorCommand(motor, 1000));
    RETURN_ON_ERROR(module_.setAcceleration(motor, 1000));
    RETURN_ON_ERROR(module_.setVelocity(motor, 0));
    RETURN_ON_ERROR(module_.setDeceleration(motor, 1000));
    RETURN_ON_ERROR(module_.update(motor));
    return ResultCode::Success;
}

void SimpleER1Interface::Impl::disconnect() {
    module_.disconnect();
}

void SimpleER1Interface::Impl::setSpeed(float speed) {

}

Result SimpleER1Interface::Impl::move(Direction dir) {
    stop();
    int vel = speed_;
    if (dir == Direction::Backward) {
        vel = -vel;
    }
    RETURN_ON_ERROR(module_.setVelocity(Motor::One, vel));
    RETURN_ON_ERROR(module_.setMotorCommand(Motor::One, 10000));

    RETURN_ON_ERROR(module_.setVelocity(Motor::Two, -vel));
    RETURN_ON_ERROR(module_.setMotorCommand(Motor::Two, 10000));

    RETURN_ON_ERROR(module_.update(Motor::One));
    RETURN_ON_ERROR(module_.update(Motor::Two));
    return ResultCode ::Success;
}

Result SimpleER1Interface::Impl::turn(Orientation ori) {
    stop();
    int vel = -speed_;
    if (ori == Orientation::Left) {
        vel = -vel;
    }
    RETURN_ON_ERROR(module_.setVelocity(Motor::One, vel));
    RETURN_ON_ERROR(module_.setMotorCommand(Motor::One, 10000));

    RETURN_ON_ERROR(module_.setVelocity(Motor::Two, vel));
    RETURN_ON_ERROR(module_.setMotorCommand(Motor::Two, 10000));

    RETURN_ON_ERROR(module_.update(Motor::One));
    RETURN_ON_ERROR(module_.update(Motor::Two));
    return ResultCode ::Success;
}

Result SimpleER1Interface::Impl::stop() {
    RETURN_ON_ERROR(module_.setVelocity(Motor::One, 0));
    RETURN_ON_ERROR(module_.setMotorCommand(Motor::One, 1000));


    RETURN_ON_ERROR(module_.setVelocity(Motor::Two, 0));
    RETURN_ON_ERROR(module_.setMotorCommand(Motor::Two, 1000));

    RETURN_ON_ERROR(module_.update(Motor::One));
    RETURN_ON_ERROR(module_.update(Motor::Two));

    return ResultCode::Success;
}


// ################################ SimpleER1Interface ################################
Result SimpleER1Interface::connect(std::string name) {
    return impl_->connect(std::move(name));
}

void SimpleER1Interface::disconnect() {
    return impl_->disconnect();
}

void SimpleER1Interface::setSpeed(float speed) {
    return impl_->setSpeed(speed);
}

Result SimpleER1Interface::move(Direction dir) {
    return impl_->move(dir);
}

Result SimpleER1Interface::turn(Orientation ori) {
    return impl_->turn(ori);
}

Result SimpleER1Interface::stop() {
    return impl_->stop();
}

} // namespace er1driver
