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

    inline eMessageType getMessageTypeFromMessage(const std::string& str);
    const std::string getPayloadFromMessage(const std::string& str);

   public:
    explicit Message(const std::string& str);
    explicit Message(const eMessageType e, const std::string& str = "");
    ~Message() = default;

    Message()                                = delete;
    Message& operator=(const Message& other) = delete;
    Message(const Message& other)            = delete;
    Message(Message&& other)                 = delete;
    Message& operator=(Message&& other)      = delete;

    const std::string getMessageStr() const;
    bool isKeepAlive() const;
    int getLength() const;
    eMessageType getMessageType() const;
    const std::string& getPayload() const;
};

#endif  // MESSAGE_HPP