#include "service.h"
#include <iostream>

// Rust FFI関数の宣言
extern "C" {
    void* logic_processor_new();
    void logic_processor_free(void* processor);
    Response logic_processor_process(void* processor, const char* message);
    void free_response_message(char* message);
}

// コンストラクタ - Rustのロジックプロセッサを初期化
Service::Service() {
    std::cout << "Service: 初期化中..." << std::endl;
    logic_processor = logic_processor_new();
}

// デストラクタ - Rustのリソースを解放
Service::~Service() {
    std::cout << "Service: 終了処理中..." << std::endl;
    if (logic_processor) {
        logic_processor_free(logic_processor);
        logic_processor = nullptr;
    }
}

// メッセージを処理し、コールバックで処理結果を通知
void Service::processMessage(const std::string& message, ResponseCallback callback) {
    std::cout << "Service: メッセージ処理中: '" << message << "'" << std::endl;
    
    // Rust側の処理を実行
    Response response = logic_processor_process(logic_processor, message.c_str());
    
    // コールバックを呼び出して結果を返す
    callback(response);
    
    // レスポンスのメッセージメモリを解放
    free_response_message(response.message);
} 