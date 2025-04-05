#pragma once

#include <string>
#include <functional>
#include <memory>
#include "response.h"
#include "network.h"

// サービスクラス - C++とRustの統合を担当
class Service {
public:
    // コールバック関数の型定義 - Responseを引数に取る
    using ResponseCallback = std::function<void(const Response&)>;
    
    Service();
    ~Service();
    
    // メッセージを処理し、コールバックで処理結果を通知（同期処理）
    void processMessage(const std::string& message, ResponseCallback callback);
    
    // メッセージを送信するだけ（結果は受け取らない - 同期処理）
    void send(const std::string& message);
    
    // メッセージをRustのキューに追加する（非同期処理）
    void sendToQueue(const std::string& message);
    
private:
    // ネットワークインスタンス
    std::unique_ptr<Network> network;
    
    // Rust側のLogicProcessorインスタンスへのポインタ
    void* logic_processor;
};