#include "service.h"
#include <iostream>

int main() {
    std::cout << "Starting C++ and Rust FFI Example" << std::endl;
    
    Service service;
    std::string result;
    
    // コールバック関数を使ってメッセージを処理
    service.processMessage("Hello from C++", [&result](const std::string& response) {
        result = response;
        std::cout << "Callback received response" << std::endl;
    });
    
    std::cout << "Final result: " << result << std::endl;
    
    return 0;
} 