#include "Message.hpp"

#include <arpa/inet.h>

#include <stdexcept>

#include "utils.hpp"

Message::Message(const std::string& str) : length(getIntFromStr(str))
{
    if (length)
    {
        MessageType = getMessageTypeFromMessage(str);
        payload     = getPayloadFromMessage(str);
    }
    else
        MessageType = eMessageType::KeppAlive;
}

Message::Message(const eMessageType e, const std::string& str) : length(1 + str.size()), MessageType(e), payload(str)
{
}

eMessageType Message::getMessageTypeFromMessage(const std::string& str)
{
    return static_cast<eMessageType>(static_cast<unsigned char>(str[4]));
}

const std::string Message::getPayloadFromMessage(const std::string& str)
{
    if (static_cast<int>(str.size() - 4) != length)
        throw std::runtime_error("Payload is incomplete");
    std::string res(length - 1, '\0');
    for (int i = 0; i < length - 1; ++i)
        res[i] = str[i + 5];
    return res;
}

const std::string Message::getMessageStr() const
{
    return intToBytes(htonl(length)) + static_cast<char>(MessageType) + payload;
}

bool Message::isKeepAlive() const
{
    return length == 0;
}

int Message::getLength() const
{
    return length;
}

eMessageType Message::getMessageType() const
{
    return MessageType;
}

const std::string& Message::getPayload() const
{
    return payload;
}
