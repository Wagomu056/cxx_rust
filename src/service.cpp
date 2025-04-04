#include "service.h"
#include "network.h"
#include <iostream>

// Rustから提供される関数のプロトタイプ宣言
extern "C" {
    // LogicProcessor関連の関数
    LogicProcessor* logic_processor_new();
    void logic_processor_free(LogicProcessor* processor);
    char* logic_processor_process(const LogicProcessor* processor, const char* input);
    
    // 文字列メモリ解放用関数
    void free_rust_string(char* ptr);
}

Service::Service() {
    std::cout << "Service initialized" << std::endl;
    // LogicProcessorインスタンスを作成
    logic_processor = logic_processor_new();
}

Service::~Service() {
    std::cout << "Service destroyed" << std::endl;
    // LogicProcessorインスタンスを破棄
    if (logic_processor) {
        logic_processor_free(logic_processor);
        logic_processor = nullptr;
    }
}

std::string Service::processMessage(const std::string& message) {
    std::cout << "Service: Processing message: " << message << std::endl;
    
    // Rustのロジックを呼び出す
    char* result = logic_processor_process(logic_processor, message.c_str());
    
    // 結果をC++の文字列に変換
    std::string cpp_result(result);
    
    // Rustが確保したメモリを解放
    free_rust_string(result);
    
    // 処理結果をネットワーク層に渡す
    Network network;
    return network.send(cpp_result);
} 