#include "network.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <vector>
#include <mutex>
#include <cstring>
#include <cstdlib> // strdup用

// 静的メンバ変数の初期化
std::vector<std::string> Network::message_log;
std::mutex Network::log_mutex;

Network::Network() {
    std::cout << "Network initialized" << std::endl;
}

Network::~Network() {
    std::cout << "Network destroyed" << std::endl;
}

std::string Network::send(const std::string& message) {
    std::cout << "Network: Sending message: " << message << std::endl;
    
    // メッセージをログに記録
    {
        std::lock_guard<std::mutex> lock(log_mutex);
        message_log.push_back(message);
    }
    
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

// 記録されたメッセージを取得するメソッド
std::vector<std::string> Network::getMessageLog() {
    std::lock_guard<std::mutex> lock(log_mutex);
    return message_log;
}

// メッセージログをクリアするメソッド
void Network::clearMessageLog() {
    std::lock_guard<std::mutex> lock(log_mutex);
    message_log.clear();
}

// C言語インターフェース実装

// Networkオブジェクトでメッセージを送信する（C互換）
extern "C" const char* network_send(Network* network, const char* message) {
    if (!network || !message) {
        return nullptr;
    }
    
    // C++のstd::stringに変換
    std::string cpp_message(message);
    
    // 送信処理を実行
    std::string result = network->send(cpp_message);
    
    // strdupを使用して文字列を複製
    return strdup(result.c_str());
}

// Cの文字列を解放する関数
extern "C" void free_network_string(char* ptr) {
    if (ptr) {
        free(ptr); // strdupで確保したメモリはfreeで解放
    }
} 