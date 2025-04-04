#include "network.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <future>

Network::Network() {
    std::cout << "Network initialized" << std::endl;
}

Network::~Network() {
    std::cout << "Network destroyed" << std::endl;
}

std::string Network::send(const std::string& message) {
    std::cout << "Network: Sending message: " << message << std::endl;
    
    // 非同期処理を作成（別スレッドで実行）
    auto future = std::async(std::launch::async, [message]() {
        std::cout << "Network: Processing in background thread..." << std::endl;
        
        // 0.3秒待機
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        
        return "Response: " + message + " (processed by network after delay)";
    });
    
    // 非同期処理の結果を待機して取得
    return future.get();
} 