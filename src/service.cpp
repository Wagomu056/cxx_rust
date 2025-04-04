#include "service.h"
#include "network.h"
#include <iostream>

// Rustから提供される関数のプロトタイプ宣言
extern "C" {
    char* process_logic(const char* input);
    void free_rust_string(char* ptr);
}

Service::Service() {
    std::cout << "Service initialized" << std::endl;
}

Service::~Service() {
    std::cout << "Service destroyed" << std::endl;
}

std::string Service::processMessage(const std::string& message) {
    std::cout << "Service: Processing message: " << message << std::endl;
    
    // Rustのロジックを呼び出す
    char* result = process_logic(message.c_str());
    
    // 結果をC++の文字列に変換
    std::string cpp_result(result);
    
    // Rustが確保したメモリを解放
    free_rust_string(result);
    
    // 処理結果をネットワーク層に渡す
    Network network;
    return network.send(cpp_result);
} 