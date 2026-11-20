#pragma once
#include <string>
#include <unordered_map>

enum class SipMethod
{
    INVITE,
    ACK,
    BYE,
    CANCEL,
    REGISTER,
    OPTIONS,
    UNKNOWN
};

struct SipMessage
{
    bool isRequest = true;
    SipMethod method = SipMethod::UNKNOWN; // Loai ban tin neu la request
    std::string requestUri;                // Dia chi dich
    std::string sipVersion = "SIP/2.0";
    int statusCode = 0;       // Ma trang thai neu la Response
    std::string reasonPhrase; // Mo ta trang thai neu la Response, vi du "OK"
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    std::string getHeader(const std::string &key) const
    {
        auto it = headers.find(key);
        if (it != headers.end())
        {
            return it->second;
        }
        return "";
    }
};