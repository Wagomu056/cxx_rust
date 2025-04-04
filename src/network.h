#pragma once

#include <string>
#include <vector>
#include <mutex>
#include "response.h"

// ネットワーク処理クラス
class Network {
private:
    // 送信されたメッセージのログ
    static std::vector<std::string> message_log;
    static std::mutex log_mutex;

public:
    Network();
    ~Network();

    // メッセージ送信メソッド（モック）
    Response send(const std::string& message);

    // 記録されたメッセージを取得するメソッド
    static std::vector<std::string> getMessageLog();
    
    // メッセージログをクリアするメソッド
    static void clearMessageLog();
};

// C言語インターフェース（FFI用）
extern "C" {
    // Networkオブジェクトでメッセージを送信するメソッド（C互換）
    Response network_send(Network* network, const char* message);
} 