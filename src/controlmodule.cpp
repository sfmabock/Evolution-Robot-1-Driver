#include <er1driver/controlmodule.hpp>
#include <linux/serial.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <er1driver/result.hpp>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <er1driver/stringify.hpp>
#include <er1driver/commandresponse.hpp>
#include <er1driver/commandquery.hpp>
#include <iostream>
#include <string>
#include <cstring>


namespace er1driver {

Result ControlModule::connect(std::string deviceName) {
    deviceName_ = std::move(deviceName);
    if ((fd_ = open(deviceName_.c_str(), O_RDWR | O_NONBLOCK | O_NDELAY, S_IRUSR | S_IWUSR)) < 0) {
        return stringify("Could not open file handle. ", std::strerror(errno));
    }
    int flags = 0;
    if ((flags = fcntl(fd_, F_GETFL)) < 0) {
        disconnect();
        return stringify("Could not modify file handle. ", std::strerror(errno));
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd_, F_SETFL, flags) < 0) {
        disconnect();
        return stringify("Could not modify file handle. ", std::strerror(errno));
    }

    // baud

    struct termios term;

    if (tcgetattr(fd_, &term) < 0) {
        return stringify("Could not modify file handle. ", std::strerror(errno));
    }

    cfmakeraw(&term);
    cfsetispeed(&term, B38400);
    cfsetospeed(&term, B38400);


    if (tcsetattr(fd_, TCSADRAIN, &term) < 0) {
        disconnect();
        return stringify("Could not modify file handle. ", std::strerror(errno));
    }

    struct serial_struct serial_info;
    serial_info.flags = ASYNC_SPD_CUST | ASYNC_LOW_LATENCY;
    serial_info.custom_divisor = (int) (96);

    if (ioctl(fd_, TIOCSSERIAL, &serial_info) < 0) {
        return stringify("Could not modify file handle. ", std::strerror(errno));
    }

    return ResultCode::Success;
}

Result ControlModule::writeToRMC(const Bytes &data) {
    auto start = std::chrono::high_resolution_clock::now();
    auto timeout = std::chrono::seconds(5);
    int totalWritten = 0;
    while (true) {
        auto written = write(fd_, &data[0] + totalWritten, data.size() - totalWritten);
        if (written < 0) {
            if (errno != EAGAIN) {
                return stringify("File IO Error. ", strerror(errno));
            }
        }
        totalWritten += written;
        if (totalWritten == data.size()) {
            break;
        }
        auto now = std::chrono::high_resolution_clock::now();
        if (now - start > timeout) {
            return "Serial port write io timeout";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return ResultCode::Success;
}

Result ControlModule::readFromRMC(Bytes *dest, int count) {
    auto start = std::chrono::high_resolution_clock::now();
    auto timeout = std::chrono::seconds(20);
    Bytes data;
    data.resize(count);
    int totalRead = 0;
    while (true) {
        auto thisRead = read(fd_, &data[0] + totalRead, count - totalRead);
        if (thisRead < 0) {
            if (errno != EAGAIN) {
                return stringify("File IO Error. ", strerror(errno));
            }
        } else {
            auto msg = stringify("Read ", thisRead, " bytes");
            totalRead += thisRead;
            if (totalRead == count) {
                break;
            }
        }
        auto now = std::chrono::high_resolution_clock::now();
        if (now - start > timeout) {
            return "Serial port read io timeout";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    *dest = std::move(data);
    return ResultCode::Success;
}

bool ControlModule::isConnected() const {
    return fd_ > 0;
}

void ControlModule::disconnect() {
    close(fd_);
    fd_ = -1;
}
Result ControlModule::getVersion(CMDGetVersionResult *dest, Motor motor) {
    CommandQuery query;
    query.init(motor, MCUCommand::GetVersion, PayloadSize::Zero);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    Bytes bytes;
    RETURN_ON_ERROR(readFromRMC(&bytes, 6));
    CommandResponse response;
    response.init(bytes);

    CMDGetVersionResult result;
    result.specialAttributes = response.getPayloadBits(Word::One, BytePosition::Low_0_7, 0, 3);
    result.numberOfAxes = response.getPayloadBits(Word::One, BytePosition::Low_0_7, 4, 7);
    result.motorType = response.getPayloadBits(Word::One, BytePosition::High_8_15, 8, 11);
    result.productFamily = response.getPayloadBits(Word::One, BytePosition::High_8_15, 12, 15);

    result.minorSoftwareVersion = response.getPayloadBits(Word::Two, BytePosition::Low_0_7, 0, 3);
    result.majorSoftwareVersion = response.getPayloadBits(Word::Two, BytePosition::Low_0_7, 4, 7);
    result.customizationCode = response.getPayloadBits(Word::Two, BytePosition::High_8_15, 8, 15);
    *dest = result;
    return ResultCode::Success;
}

Result ControlModule::getEventStatus(CMDGetEventStatusResult *dest, Motor motor) {
    CommandQuery query;
    query.init(motor, MCUCommand::GetEventStatus, PayloadSize::Zero);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    Bytes bytes;
    RETURN_ON_ERROR(readFromRMC(&bytes, 4));
    CommandResponse response;
    response.init(bytes);

    CMDGetEventStatusResult result;
    result.motionComplete = response.getPayloadBits(Word::One, BytePosition::Low_0_7, 0, 0);
    result.wrapAround = response.getPayloadBits(Word::One, BytePosition::Low_0_7, 1, 1);
    result.breakpoint1 = response.getPayloadBits(Word::One, BytePosition::Low_0_7, 2, 2);
    result.captureReceived = response.getPayloadBits(Word::One, BytePosition::Low_0_7, 3, 3);
    result.motionError = response.getPayloadBits(Word::One, BytePosition::Low_0_7, 4, 4);
    result.inPositiveLimit = response.getPayloadBits(Word::One, BytePosition::Low_0_7, 5, 5);
    result.inNegativLimit = response.getPayloadBits(Word::One, BytePosition::Low_0_7, 6, 6);
    result.instructionError = response.getPayloadBits(Word::One, BytePosition::Low_0_7, 7, 7);

    result.communicationError = response.getPayloadBits(Word::One, BytePosition::High_8_15, 11, 11);
    result.breakpoint2 = response.getPayloadBits(Word::One, BytePosition::High_8_15, 14, 14);


    *dest = result;
    return ResultCode::Success;
}

Result ControlModule::getProfileMode(ProfileMode *mode, Motor motor) {
    CommandQuery query;
    query.init(motor, MCUCommand::GetProfileMode, PayloadSize::Zero);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    Bytes bytes;
    RETURN_ON_ERROR(readFromRMC(&bytes, 4));
    CommandResponse response;
    response.init(bytes);

    *mode = static_cast<ProfileMode>(response.getPayloadBits(Word::One, BytePosition::Low_0_7, 0, 2));
    return ResultCode::Success;
}

Result ControlModule::readAnalog(Motor motor, int portid, std::uint16_t *value) {
    CommandQuery query;
    query.init(motor, MCUCommand::ReadAnalog, PayloadSize::OneWord);
    query.setByteValue(Word::One, BytePosition::Low_0_7, portid);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    Bytes bytes;
    RETURN_ON_ERROR(readFromRMC(&bytes, 4));
    CommandResponse response;
    response.init(bytes);


    *value = static_cast<std::uint16_t >(response.getPayloadWord(Word::One));
    return ResultCode::Success;
}

Result ControlModule::setProfileMode(Motor motor, ProfileMode mode) {
    CommandQuery query;
    query.init(motor, MCUCommand::SetProfileMode, PayloadSize::OneWord);
    query.setByteValue(Word::One, BytePosition::Low_0_7, static_cast<std::uint8_t>(mode));
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    RETURN_ON_ERROR(getAck());
    return ResultCode::Success;
}

Result ControlModule::setLimitSwitchMode(Motor motor, LimitSwitchMode mode) {
    CommandQuery query;
    query.init(motor, MCUCommand::SetLimitSwitchMode, PayloadSize::OneWord);
    query.setByteValue(Word::One, BytePosition::Low_0_7, static_cast<std::uint8_t>(mode));
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    RETURN_ON_ERROR(getAck());
    return ResultCode::Success;
}

Result ControlModule::setVelocity(Motor motor, int velocity) {
    CommandQuery query;
    query.init(motor, MCUCommand::SetVelocity, PayloadSize::TwoWords);
    query.setIntAsPayload(velocity);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    RETURN_ON_ERROR(getAck());
    return ResultCode::Success;
}

Result ControlModule::setAcceleration(er1driver::Motor motor, int acc) {
    CommandQuery query;
    query.init(motor, MCUCommand::SetAcceleration, PayloadSize::TwoWords);
    query.setIntAsPayload(acc);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    RETURN_ON_ERROR(getAck());
    return ResultCode::Success;
}

Result ControlModule::setDeceleration(Motor motor, std::uint32_t value) {
    CommandQuery query;
    query.init(motor, MCUCommand::SetDeceleration, PayloadSize::TwoWords);
    query.setIntAsPayload(value);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    RETURN_ON_ERROR(getAck());
    return ResultCode::Success;
}

Result ControlModule::setPosition(Motor motor, int position) {
    CommandQuery query;
    query.init(motor, MCUCommand::SetPosition, PayloadSize::TwoWords);
    query.setIntAsPayload(position);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    RETURN_ON_ERROR(getAck());
    return ResultCode::Success;
}

Result ControlModule::getPosition(Motor motor, int *position) {
    CommandQuery query;
    query.init(motor, MCUCommand::GetPosition, PayloadSize::Zero);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    Bytes bytes;
    RETURN_ON_ERROR(readFromRMC(&bytes, 6));
    CommandResponse response;
    response.init(bytes);
    *position = response.getPayloadInt();
    return ResultCode::Success;
}


Result ControlModule::reset(Motor motor) {
    CommandQuery query;
    query.init(motor, MCUCommand::Reset, PayloadSize::Zero);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    RETURN_ON_ERROR(getAck());
    return ResultCode::Success;
}

Result ControlModule::update(Motor motor) {
    CommandQuery query;
    query.init(motor, MCUCommand::Update, PayloadSize::Zero);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    RETURN_ON_ERROR(getAck());
    return ResultCode::Success;
}

Result ControlModule::setMotorCommand(Motor motor, std::int16_t value) {
    CommandQuery query;
    query.init(motor, MCUCommand::SetMotorCommand, PayloadSize::OneWord);
    query.setWordValue(Word::One, value);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    RETURN_ON_ERROR(getAck());
    return ResultCode::Success;
}

Result ControlModule::resetEventStatus(er1driver::Motor motor, std::uint16_t value) {
    CommandQuery query;
    query.init(motor, MCUCommand::ResetEventStatus, PayloadSize::OneWord);
    query.setWordValue(Word::One, value);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    RETURN_ON_ERROR(getAck());
    return ResultCode::Success;
}


Result ControlModule::getMotorMode(MotorMode *mode, Motor motor) {
    CommandQuery query;
    query.init(motor, MCUCommand::GetMotorMode, PayloadSize::Zero);
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    Bytes bytes;
    RETURN_ON_ERROR(readFromRMC(&bytes, 4));
    CommandResponse response;
    response.init(bytes);
    *mode = static_cast<MotorMode>(response.getPayloadBit(Word::One, BytePosition::Low_0_7, 0));
    return ResultCode::Success;
}

Result ControlModule::setMotorMode(Motor motor, MotorMode mode) {
    CommandQuery query;
    query.init(motor, MCUCommand::SetMotorMode, PayloadSize::OneWord);
    query.setByteValue(Word::One, BytePosition::Low_0_7, static_cast<Byte>(mode));
    auto cmd = query.getFinalCommandBytes();
    RETURN_ON_ERROR(writeToRMC(cmd));
    RETURN_ON_ERROR(getAck());
    return ResultCode::Success;
}

Result ControlModule::getAck() {
    Bytes bytes;
    RETURN_ON_ERROR(readFromRMC(&bytes, 2));
    return ResultCode::Success;
}

void CommandResponse::init(Bytes bytes) {
    bytes_ = std::move(bytes);
}



} // namespace er1driver