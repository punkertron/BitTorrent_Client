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
