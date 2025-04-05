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
    
    // メッセージを処理し、コールバックで処理結果を通知
    void processMessage(const std::string& message, ResponseCallback callback);
    
private:
    // ネットワークインスタンス
    std::unique_ptr<Network> network;
    
    // Rust側のLogicProcessorインスタンスへのポインタ
    void* logic_processor;
};