#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace er1driver {

using Byte = std::uint8_t;
using Bytes = std::vector<Byte>;

enum class MCUCommand : std::uint8_t {
    GetVersion = 0x8F,
    GetEventStatus = 0x31,
    GetProfileMode = 0xA1,
    SetProfileMode = 0xA0,
    Reset = 0x39,
    Update = 0x1A,
    SetVelocity = 0x11,
    GetVelocity = 0x4B,
    SetAcceleration = 0x90,
    GetAcceleration = 0x4C,
    SetMotorCommand = 0x77,
    GetMotorCommand = 0x69,
    SetMotorMode = 0xDC,
    GetMotorMode = 0xDD,
    ResetEventStatus = 0x34,
    SetLimitSwitchMode = 0x80,
    GetLimitSwitchMode = 0x81,
    ReadAnalog = 0xEF,
    SetDeceleration = 0x91,
    GetDeceleration = 0x92,
    SetPosition = 0x10,
    GetPosition = 0x4A,
    Noop = 0x00,
};

enum class Motor : std::uint8_t {
    One = 0x00,
    Two = 0x01
};

enum class MotorMode {
    On = 0x1,
    Off = 0x0
};

enum class PayloadSize {
    Zero = 0,
    OneWord = 1,
    TwoWords = 2,
    ThreeWords = 3
};

enum class Word {
    One = 0,
    Two = 1,
    Three = 2
};

enum class BytePosition {
    Low_0_7 = 1,
    High_8_15 = 0
};

enum class ProfileMode {
    Trapezodial = 0,
    VelocityContouring  = 1,
    SCurve = 2,
    External = 4
};

enum class LimitSwitchMode {
    On = 1,
    Off = 0
};

struct CMDGetVersionResult {
    Byte specialAttributes;
    Byte numberOfAxes;
    Byte motorType;
    Byte productFamily;
    Byte minorSoftwareVersion;
    Byte majorSoftwareVersion;
    Byte customizationCode;

    std::string toString();
};

/**
 *
 */
struct CMDGetEventStatusResult {
    Byte motionComplete;
    Byte wrapAround;
    Byte breakpoint1;
    Byte captureReceived;
    Byte motionError;
    Byte inPositiveLimit;
    Byte inNegativLimit;
    Byte instructionError;
    Byte communicationError;
    Byte breakpoint2;

    std::string toString();
};

constexpr int Er1CommanHeaderSize = 2;
constexpr int Er1ResponseHeaderSize = 2;
constexpr int MC3410CommandHeaderSize = 2;
constexpr int TotalHeaderSize = Er1CommanHeaderSize + MC3410CommandHeaderSize;


} // namespace er1driver