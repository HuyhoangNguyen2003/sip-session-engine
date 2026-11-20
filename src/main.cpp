#include "Logger.hpp"
#include "SipParser.hpp"
#include <iostream>
#include <thread>
#include <vector>

void workerTask(int threadId)
{
    for (int i = 1; i <= 5; i++)
    {
        Logger::getInstance().log(
            LogLevel::INFO,
            "Thread " + std::to_string(threadId) + " - Message " + std::to_string(i));
    }
}

int main()
{

    Logger::getInstance().init("logs/sip_engine.log");
    std::cout << "----Server Start logging----\n";

    std::string rawInvite =
        "INVITE sip:bob@biloxi.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.atlanta.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: Bob <sip:bob@biloxi.com>\r\n"
        "From: Alice <sip:alice@atlanta.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.atlanta.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Content-Type: application/sdp\r\n"
        "\r\n"
        "v=0\r\n"
        "o=alice 2890844526 2890844526 IN IP4 pc33.atlanta.com\r\n";

    auto parsed = SipParser::parse(rawInvite);
    if (parsed.has_value())
    {
        Logger::getInstance().log(LogLevel::INFO, "SIP INVITE parsed successfully!");
        Logger::getInstance().log(LogLevel::INFO, "Request URI: " + parsed->requestUri);
        Logger::getInstance().log(LogLevel::INFO, "Call-ID: " + parsed->getHeader("Call-ID"));
        Logger::getInstance().log(LogLevel::INFO, "From: " + parsed->getHeader("From"));
        Logger::getInstance().log(LogLevel::INFO, "Body:\n" + parsed->body);
    }

    else
    {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to parse SIP Message");
    }

    // std::vector<std::thread> threads;
    // for (int i = 0; i < 4; i++)
    // {
    //     threads.emplace_back(workerTask, i);
    // }

    // for (auto &t : threads)
    // {
    //     t.join();
    // }
    return 0;
    
}