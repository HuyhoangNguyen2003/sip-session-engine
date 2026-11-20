#pragma once
#include "SipMessage.hpp"
#include <optional>
#include <string_view>

class SipParser
{
public:
    static std::optional<SipMessage> parse(std::string_view rawMessage);

private:
    static SipMethod stringToMethod(std::string_view methodStr);

    static std::string_view trim(std::string_view str); 

    static bool parseStartLine(std::string_view line, SipMessage &outMsg); // Phan tich dong dau tien cua ban tin SIP 
};