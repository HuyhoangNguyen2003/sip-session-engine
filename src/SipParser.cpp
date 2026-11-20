#include "SipParser.hpp"
#include <algorithm>
#include <cctype>
#include <charconv>

SipMethod SipParser::stringToMethod(std::string_view methodStr)
{
    if (methodStr == "INVITE")
    {
        return SipMethod::INVITE;
    }
    if (methodStr == "ACK")
    {
        return SipMethod::ACK;
    }
    if (methodStr == "BYE")
    {
        return SipMethod::BYE;
    }
    if (methodStr == "CANCEL")
    {
        return SipMethod::CANCEL;
    }
    if (methodStr == "REGISTER")
    {
        return SipMethod::REGISTER;
    }
    if (methodStr == "OPTIONS")
    {
        return SipMethod::OPTIONS;
    }
    else
    {
        return SipMethod::UNKNOWN;
    }
}

std::string_view SipParser::trim(std::string_view str)
{
    const std::string_view whitespace = " \t\r\n";
    const size_t first = str.find_first_not_of(whitespace);
    if (first == std::string_view::npos)
    {
        return "";
    }
    const size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, last - first + 1);
}

bool SipParser::parseStartLine(std::string_view line, SipMessage &outMsg)
{
    size_t firstSpace = line.find(' ');
    if (firstSpace == std::string_view::npos)
    {
        return false;
    }

    size_t secondSpace = line.find(' ', firstSpace + 1);
    if (secondSpace == std::string_view::npos)
    {
        return false;
    }

    std::string_view part1 = line.substr(0, firstSpace);
    std::string_view part2 = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    std::string_view part3 = trim(line.substr(secondSpace + 1));

    if (part1.rfind("SIP/", 0) == 0)
    {
        outMsg.isRequest = false;
        outMsg.sipVersion = std::string(part1);

        auto [ptr, ec] = std::from_chars(part2.data(), part2.data() + part2.size(), outMsg.statusCode);
        if (ec != std::errc())
        {
            return false;
        }

        outMsg.reasonPhrase = std::string(part3);
        return true;
    }
    else
    {
        outMsg.isRequest = true;
        outMsg.method = stringToMethod(part1);
        if (outMsg.method == SipMethod::UNKNOWN){
            return false; 
        }

        outMsg.requestUri = std::string(part2);
        outMsg.sipVersion = std::string(part3);


        return true;
    }
}

std::optional<SipMessage> SipParser::parse(std::string_view rawMessage)
{
    if (rawMessage.empty())
    {
        return std::nullopt;
    }

    SipMessage msg;

    std::string_view headerSection = rawMessage;
    size_t bodySep = rawMessage.find("\r\n\r\n");
    if (bodySep != std::string_view::npos)
    {
        headerSection = rawMessage.substr(0, bodySep);
        msg.body = std::string(rawMessage.substr(bodySep + 4));
    }
    else
    {
        bodySep = rawMessage.find("\n\n");
        if (bodySep != std::string_view::npos)
        {
            headerSection = rawMessage.substr(0, bodySep);
            msg.body = std::string(rawMessage.substr(bodySep + 2));
        }
    }

    size_t lineEnd = headerSection.find('\n');
    std::string_view firstLine = trim(headerSection.substr(0, lineEnd));
    if (!parseStartLine(firstLine, msg))
    {
        return std::nullopt;
    }

    if (lineEnd == std::string_view::npos)
    {
        return msg;
    }

    size_t pos = lineEnd + 1;
    while (pos < headerSection.size())
    {
        size_t nextLine = headerSection.find('\n', pos);
        std::string_view line = (nextLine == std::string_view::npos) ? headerSection.substr(pos) : headerSection.substr(pos, nextLine - pos);

        line = trim(line);
        if (!line.empty())
        {
            size_t colonPos = line.find(':');
            if (colonPos != std::string_view::npos)
            {
                std::string_view key = trim(line.substr(0, colonPos));
                std::string_view val = trim(line.substr(colonPos + 1));
                msg.headers[std::string(key)] = std::string(val);
            }
        }

        if (nextLine == std::string_view::npos)
        {
            break;
        }
        pos = nextLine + 1;
    }

    return msg;
}