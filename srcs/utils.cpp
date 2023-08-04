#include "utils.hpp"

const std::string sha1(const std::string& str)
{
    CryptoPP::SHA1 sha1;
    CryptoPP::byte digest[CryptoPP::SHA1::DIGESTSIZE];

    sha1.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte*>(str.c_str()), str.size());

    CryptoPP::HexEncoder encoder;
    std::string hexOutput;
    encoder.Attach(new CryptoPP::StringSink(hexOutput));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    return hexOutput;
}

std::string hexDecode(const std::string& value)
{
    int hashLength = value.length();
    std::string decodedHexString;
    for (int i = 0; i < hashLength; i += 2)
    {
        std::string byte = value.substr(i, 2);
        char c           = (char)(int)strtol(byte.c_str(), nullptr, 16);
        decodedHexString.push_back(c);
    }
    return decodedHexString;
}

std::string urlEncodeHex(const std::string& input)
{
    std::string result;
    const char* hexDigits = "0123456789ABCDEF";

    for (char ch : input)
    {
        if (isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~')
        {
            result += ch;
        }
        else
        {
            result += '%';
            result += hexDigits[(ch >> 4) & 0x0F];
            result += hexDigits[ch & 0x0F];
        }
    }

    return result;
}

std::string bytesToIPAddress(const std::string& bytes)
{
    if (bytes.length() < 4)
        return "";

    std::string ipAddress;
    for (int i = 0; i < 4; ++i)
    {
        unsigned int byteValue = static_cast<unsigned char>(bytes[i]);
        ipAddress += std::to_string(byteValue);
        if (i < 3)
            ipAddress += ".";
    }

    return ipAddress;
}

long long bytesToPort(const std::string& bytes)
{
    if (bytes.length() < 2)
        return 0;

    long long port = 0;
    port |= static_cast<unsigned char>(bytes[0]) << 8;
    port |= static_cast<unsigned char>(bytes[1]);

    return port;
}

int getLengthFromMessage(const std::string& str)
{
    if (str.size() < 4)
    {
        throw std::runtime_error("String does not contain 4 bytes");
    }

    unsigned int result = (static_cast<unsigned char>(str[0]) << 24) | (static_cast<unsigned char>(str[1]) << 16) |
                          (static_cast<unsigned char>(str[2]) << 8) | (static_cast<unsigned char>(str[3]));

    return static_cast<int>(result);
}

std::string intToBytes(int x)
{
    std::string bytes(sizeof(int), '\0');
    std::memcpy(&bytes[0], &x, sizeof(int));
    return bytes;
}
