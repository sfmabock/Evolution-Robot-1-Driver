#include <er1driver/commandquery.hpp>
#include <arpa/inet.h>

namespace er1driver {

void CommandQuery::init(er1driver::Motor motor, er1driver::MCUCommand cmd, er1driver::PayloadSize size) {
    auto payloadByteSize = static_cast<int>(size) * 2;
    auto totalSize = TotalHeaderSize + payloadByteSize;
    cmd_.resize(totalSize, 0);
    cmd_[0] = static_cast<std::uint8_t>(motor);
    cmd_[1] = 0; // checksum will be set later
    cmd_[2] = 0;
    cmd_[3] = static_cast<Byte>(cmd);
}

Bytes CommandQuery::getFinalCommandBytes() {
    int sum = 0x100;
    for (int i = 0; i < cmd_.size(); ++i) {
        if (i == 1) {
            continue;
        }
        sum -= cmd_[i];
    }
    cmd_[1] = static_cast<std::uint8_t>(sum);
    return cmd_;
}

void CommandQuery::setByteValue(er1driver::Word word, er1driver::BytePosition position, std::uint8_t byte) {
    auto idx = TotalHeaderSize + (2 * static_cast<int>(word) + static_cast<int>(position));
    cmd_[idx] = byte;
}

void CommandQuery::setWordValue(Word word, std::uint16_t value) {
    auto idx = TotalHeaderSize + (2 * static_cast<int>(word));
    std::uint16_t *ptr = reinterpret_cast<std::uint16_t *>(&cmd_[idx]);
    *ptr = htons(value);
}

void CommandQuery::setIntAsPayload(std::int32_t value) {
    auto idx = TotalHeaderSize;
    std::int32_t *ptr = reinterpret_cast<std::int32_t *>(&cmd_[idx]);
    *ptr = static_cast<std::uint32_t >(htonl(value));
}

} // namespace er1driver
