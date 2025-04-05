#include "network.h"
#include "response.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <vector>
#include <mutex>
#include <cstring>
#include <cstdlib> // strdup用
#include <unordered_map>

// 静的メンバ変数の初期化
std::vector<std::string> Network::message_log;
std::mutex Network::log_mutex;

// コールバックを保存するためのマップとそのミューテックス
static std::unordered_map<uint64_t, Network::ResponseCallback> callback_map;
static std::mutex callback_mutex;

Network::Network() {
    std::cout << "Network initialized" << std::endl;
}

Network::~Network() {
    std::cout << "Network destroyed" << std::endl;
}

void Network::send(const std::string& message, ResponseCallback callback) {
    std::cout << "Network: Sending message: " << message << std::endl;
    
    // メッセージをログに記録
    {
        std::lock_guard<std::mutex> lock(log_mutex);
        message_log.push_back(message);
    }
    
    // 別スレッドで非同期処理を実行
    std::thread([message, callback]() {
        std::cout << "Network: Processing in background thread..." << std::endl;
        
        // 0.3秒待機
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        
        std::string result = "Response: " + message + " (processed by network after delay)";
        
        // Responseを作成
        Response response;
        response.success = true;
        response.response_code = 200;
        response.message = strdup(result.c_str()); // コピーして返す必要があります
        
        // コールバックを呼び出して結果を返す
        callback(response);
        
        // メッセージのメモリを解放する必要はない（コールバック側で責任を持つ）
    }).detach(); // スレッドをデタッチして非同期実行
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
extern "C" void network_send(Network* network, const char* message, uint64_t callback_id, NetworkResponseCallback callback) {
    if (!network || !message || !callback) {
        if (callback) {
            Response error_response;
            error_response.success = false;
            error_response.response_code = 400;
            error_response.message = strdup("Invalid network or message pointer");
            callback(callback_id, error_response);
        }
        return;
    }
    
    // コールバックをマップに保存
    {
        std::lock_guard<std::mutex> lock(callback_mutex);
        callback_map[callback_id] = [callback_id, callback](const Response& response) {
            // C互換コールバックを呼び出す
            callback(callback_id, response);
        };
    }
    
    // C++のstd::stringに変換
    std::string cpp_message(message);
    
    // 送信処理を実行
    network->send(cpp_message, [callback_id](const Response& response) {
        // 保存したコールバックを取得
        Network::ResponseCallback stored_callback = nullptr;
        {
            std::lock_guard<std::mutex> lock(callback_mutex);
            auto it = callback_map.find(callback_id);
            if (it != callback_map.end()) {
                stored_callback = it->second;
                callback_map.erase(it);
            }
        }
        
        // コールバックを呼び出す
        if (stored_callback) {
            stored_callback(response);
        }
    });
} 