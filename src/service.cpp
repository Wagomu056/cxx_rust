#include "service.h"
#include <iostream>

// Rust FFI関数の宣言
extern "C" {
    void* logic_processor_new(Network* network);
    void logic_processor_free(void* processor);
    void logic_processor_queue_message(void* processor, const char* message);
}

// コンストラクタ - Rustのロジックプロセッサを初期化
Service::Service() {
    std::cout << "Service: 初期化中..." << std::endl;
    
    // ネットワークインスタンスを作成
    network = std::make_unique<Network>();
    
    // ネットワークインスタンスを渡してロジックプロセッサを初期化
    logic_processor = logic_processor_new(network.get());
}

// デストラクタ - Rustのリソースを解放
Service::~Service() {
    std::cout << "Service: 終了処理中..." << std::endl;
    if (logic_processor) {
        logic_processor_free(logic_processor);
        logic_processor = nullptr;
    }
    // networkはスマートポインタなので自動的に解放される
}

// メッセージをRustのキューに追加する（非同期処理）
void Service::sendToQueue(const std::string& message) {
    std::cout << "Service: メッセージをキューに追加: '" << message << "'" << std::endl;
    
    // Rust側のキュー処理を実行
    logic_processor_queue_message(logic_processor, message.c_str());
    
    std::cout << "Service: メッセージをキューに追加完了" << std::endl;
} 