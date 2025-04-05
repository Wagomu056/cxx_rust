#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <functional>
#include <cstdint>
#include "response.h"

// ネットワーク処理クラス
class Network {
public:
    // コールバック関数の型定義 - Responseを引数に取る
    using ResponseCallback = std::function<void(const Response&)>;

private:
    // 送信されたメッセージのログ
    static std::vector<std::string> message_log;
    static std::mutex log_mutex;

public:
    Network();
    ~Network();

    // メッセージ送信メソッド（モック）- コールバックベース
    void send(const std::string& message, ResponseCallback callback);

    // 記録されたメッセージを取得するメソッド
    static std::vector<std::string> getMessageLog();
    
    // メッセージログをクリアするメソッド
    static void clearMessageLog();
};

// C言語インターフェース（FFI用）
extern "C" {
    // コールバック関数の型定義（C互換）
    typedef void (*NetworkResponseCallback)(uint64_t callback_id, Response response);
    
    // Rust側から呼び出されるコールバック関数
    void rust_network_callback(uint64_t callback_id, Response response);
    
    // Networkオブジェクトでメッセージを送信するメソッド（C互換）
    void network_send(Network* network, const char* message, uint64_t callback_id, NetworkResponseCallback callback);
} 