#pragma once

#include <string>

// サービスインターフェース
class Service {
public:
    Service();
    ~Service();

    // メッセージ処理メソッド
    std::string processMessage(const std::string& message);
}; 