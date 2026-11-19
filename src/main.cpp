#include "Logger.hpp"
#include<iostream>
#include <thread>
#include <vector>


void workerTask(int threadId)
{
    for (int i = 1; i <= 5; i++)
    {
        Logger::getInstance().log(
            LogLevel::INFO, 
            "Thread " + std::to_string(threadId) + " - Message " + std::to_string(i)
        );
    }
}

int main()
{

    Logger::getInstance().init("logs/sip_engine.log"); 
    std::cout << "----Server Start logging----\n"; 

    std::vector<std::thread> threads; 
    for(int i = 0; i < 4; i++){ 
        threads.emplace_back(workerTask, i);
    }

    for(auto& t : threads){
        t.join(); 
    }
    return 0;
}