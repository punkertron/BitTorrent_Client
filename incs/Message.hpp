#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>

enum class eMessageType : int
{
    KeppAlive     = -1,
    Choke         = 0,
    Unchoke       = 1,
    Interested    = 2,
    NotInterested = 3,
    Have          = 4,
    Bitfield      = 5,
    Request       = 6,
    Piece         = 7,
    Cancel        = 8
};

class Message
{
   private:
    int length;
    eMessageType MessageType;
    std::string payload;

    eMessageType getMessageTypeFromMessage(const std::string& str);
    const std::string getPayloadFromMessage(const std::string& str);

   public:
    Message(const std::string& str);
    ~Message() = default;

    inline bool isKeepAlive() const
    {
        return length == 0;
    }

    inline const int getLength() const
    {
        return length;
    }

    inline eMessageType getMessageType() const
    {
        return MessageType;
    }

    inline const std::string& getPayload() const
    {
        return payload;
    }
};

#endif  // MESSAGE_HPP