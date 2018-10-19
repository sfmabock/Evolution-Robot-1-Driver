#pragma once
#include <er1driver/misc.hpp>

namespace er1driver {

class CommandResponse {
public:
    void init(Bytes data);

    Byte getPayloadByte(Word word, BytePosition position);
    std::uint16_t getPayloadWord(Word word);
    Byte getPayloadBits(Word word, BytePosition position, int start, int stop);
    Byte getPayloadBit(Word word, BytePosition position, int start);
    std::int32_t getPayloadInt();

private:
    Bytes bytes_;
};

} // namespace er1driver