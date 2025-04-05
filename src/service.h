#pragma once

#include <string>
#include <memory>
#include "network.h"

// サービスクラス - C++とRustの統合を担当
class Service {
public:
    Service();
    ~Service();
    
    // メッセージをRustのキューに追加する（非同期処理）
    void sendToQueue(const std::string& message);
    
private:
    // ネットワークインスタンス
    std::unique_ptr<Network> network;
    
    // Rust側のLogicProcessorインスタンスへのポインタ
    void* logic_processor;
};