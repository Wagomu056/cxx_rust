#include "service.h"
#include <iostream>

int main() {
    std::cout << "Starting C++ and Rust FFI Example" << std::endl;
    
    Service service;
    std::string result = service.processMessage("Hello from C++");
    
    std::cout << "Final result: " << result << std::endl;
    
    return 0;
} 