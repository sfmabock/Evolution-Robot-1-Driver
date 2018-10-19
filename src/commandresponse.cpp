#include <er1driver/commandresponse.hpp>
#include <arpa/inet.h>

namespace er1driver {
Byte CommandResponse::getPayloadByte(Word word, BytePosition position) {
    auto idx = Er1ResponseHeaderSize + (2 * static_cast<int>(word) + static_cast<int>(position));
    return bytes_[idx];
}

std::uint16_t CommandResponse::getPayloadWord(Word word) {
    auto headerSize = 2;
    auto idx = headerSize + (2 * static_cast<int>(word));
    return *reinterpret_cast<std::uint16_t *>(&bytes_[idx]);
}

std::int32_t CommandResponse::getPayloadInt() {
    auto idx = Er1ResponseHeaderSize;
    return ntohl(*reinterpret_cast<std::int32_t *>(&bytes_[idx]));
}

Byte CommandResponse::getPayloadBits(Word word, BytePosition position, int start, int stop) {
    // There are faster ways, but for the sake of clarity ...
    auto byte = getPayloadByte(word, position);
    start = start % 8;
    stop = stop % 8;
    Byte mask = 0;
    int span = stop - start + 1;
    for (int i = start; i < start + span; ++i) {
        mask = mask | ((Byte(1) << i));
    }
    auto res = byte & mask;
    res = res >> start;
    return res;
}

Byte CommandResponse::getPayloadBit(Word word, BytePosition position, int start) {
    return getPayloadBits(word, position, start, start);
}

} // namespace er1driver