#pragma once
#include <er1driver/result.hpp>
#include <er1driver/misc.hpp>

namespace er1driver {

class ControlModule {
public:
    Result connect(std::string deviceName);
    bool isConnected() const;
    void disconnect();

    Result getVersion(CMDGetVersionResult* dest, Motor motor);
    Result getEventStatus(CMDGetEventStatusResult* dest, Motor motor);

    Result getProfileMode(ProfileMode* mode, Motor motor);
    Result setProfileMode(Motor motor, ProfileMode mode);

    Result reset(Motor motor);
    Result update(Motor motor);
    Result getAck();

    Result setVelocity(Motor motor, int vel);
    Result setAcceleration(Motor motor, int acc);

    Result setDeceleration(Motor motor, std::uint32_t value);
    Result setPosition(Motor motor, int position);
    Result getPosition(Motor motor, int* position);

    Result setMotorCommand(Motor motor, std::int16_t value);


    Result getMotorMode(MotorMode* mode, Motor motor);
    Result setMotorMode(Motor motor, MotorMode mode);

    Result resetEventStatus(Motor motor, std::uint16_t value);
    Result setLimitSwitchMode(Motor motor, LimitSwitchMode mode);

    Result readAnalog(Motor motor, int portid, std::uint16_t* value);

private:
    Result writeToRMC(const Bytes& data);
    Result readFromRMC(Bytes* dest, int count);

    std::string deviceName_;
    int fd_ = -1;
};

}