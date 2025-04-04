#include "network.h"
#include <iostream>

Network::Network() {
    std::cout << "Network initialized" << std::endl;
}

Network::~Network() {
    std::cout << "Network destroyed" << std::endl;
}

std::string Network::send(const std::string& message) {
    std::cout << "Network: Sending message: " << message << std::endl;
    
    // 実際のネットワーク処理はモックとして処理
    return "Response: " + message + " (processed by network)";
} 