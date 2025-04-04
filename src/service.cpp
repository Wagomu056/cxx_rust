#include "service.h"
#include "network.h"
#include <iostream>

// Rustから提供される関数のプロトタイプ宣言
extern "C" {
    // LogicProcessor関連の関数
    LogicProcessor* logic_processor_new();
    void logic_processor_free(LogicProcessor* processor);
    char* logic_processor_process(const LogicProcessor* processor, Network* network, const char* input);
    
    // 文字列メモリ解放用関数
    void free_rust_string(char* ptr);
}

Service::Service() {
    std::cout << "Service initialized" << std::endl;
    // LogicProcessorインスタンスを作成
    logic_processor = logic_processor_new();
    // Networkインスタンスを作成
    network = new Network();
}

Service::~Service() {
    std::cout << "Service destroyed" << std::endl;
    // LogicProcessorインスタンスを破棄
    if (logic_processor) {
        logic_processor_free(logic_processor);
        logic_processor = nullptr;
    }
    // Networkインスタンスを破棄
    if (network) {
        delete network;
        network = nullptr;
    }
}

// コールバックを利用するバージョン
void Service::processMessage(const std::string& message, ResultCallback callback) {
    std::cout << "Service: Processing message: " << message << std::endl;
    
    // Rustのロジックを呼び出す（Networkポインタも渡す）
    char* result = logic_processor_process(logic_processor, network, message.c_str());
    
    // 結果をC++の文字列に変換
    std::string cpp_result(result);
    
    // Rustが確保したメモリを解放
    free_rust_string(result);
    
    // コールバック関数を通じて結果を返す
    callback(cpp_result);
} 