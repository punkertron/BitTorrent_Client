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