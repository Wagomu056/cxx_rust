#pragma once

#include <string>
#include <vector>
#include <mutex>

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
    std::string send(const std::string& message);

    // 記録されたメッセージを取得するメソッド
    static std::vector<std::string> getMessageLog();
    
    // メッセージログをクリアするメソッド
    static void clearMessageLog();
};

// C言語インターフェース（FFI用）
extern "C" {
    // Networkオブジェクトを送信するメソッド（C互換）
    const char* network_send(Network* network, const char* message);
    
    // Cの文字列を解放する関数（C互換）
    void free_network_string(char* ptr);
} 