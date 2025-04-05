#include "service.h"
#include <iostream>

// Rust FFI関数の宣言
extern "C" {
    void* logic_processor_new(Network* network);
    void logic_processor_free(void* processor);
    Response logic_processor_process(void* processor, const char* message);
    Response logic_processor_queue_message(void* processor, const char* message);
    void free_response(Response* response);
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

// メッセージを処理し、コールバックで処理結果を通知（同期処理）
void Service::processMessage(const std::string& message, ResponseCallback callback) {
    std::cout << "Service: メッセージ処理中: '" << message << "'" << std::endl;
    
    // Rust側の処理を実行
    Response response = logic_processor_process(logic_processor, message.c_str());
    
    // コールバックを呼び出して結果を返す
    callback(response);
    
    // レスポンスのメモリを解放
    free_response(&response);
}

// メッセージを送信するだけ（結果は受け取らない - 同期処理）
void Service::send(const std::string& message) {
    std::cout << "Service: 送信のみ実行: '" << message << "'" << std::endl;
    
    // Rust側の処理を実行
    Response response = logic_processor_process(logic_processor, message.c_str());
    
    // レスポンスは使用せずにメモリを解放
    std::cout << "Service: 送信完了（レスポンスを無視）" << std::endl;
    free_response(&response);
}

// メッセージをRustのキューに追加する（非同期処理）
void Service::sendToQueue(const std::string& message) {
    std::cout << "Service: メッセージをキューに追加: '" << message << "'" << std::endl;
    
    // Rust側のキュー処理を実行
    Response response = logic_processor_queue_message(logic_processor, message.c_str());
    
    // キューにメッセージが追加されたことを確認
    if (response.success) {
        std::cout << "Service: メッセージをキューに追加完了: " << response.message << std::endl;
    } else {
        std::cerr << "Service: キュー追加に失敗: " << response.message << std::endl;
    }
    
    // レスポンスのメモリを解放
    free_response(&response);
} 