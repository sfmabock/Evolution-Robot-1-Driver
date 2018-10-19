#pragma once
#include <er1driver/misc.hpp>
#include <cstdint>

namespace er1driver {

class CommandQuery {
public:
    void init(Motor motor, MCUCommand cmd, PayloadSize size);

    void setByteValue(Word word, BytePosition position, std::uint8_t byte);
    void setWordValue(Word word, std::uint16_t value);
    void setIntAsPayload(std::int32_t value);

    Bytes getFinalCommandBytes();

private:
    Bytes cmd_;
};


} // namespace er1driver