#pragma once

#include <string>

// 前方宣言（Rustの構造体用）
typedef struct LogicProcessor LogicProcessor;

// サービスインターフェース
class Service {
private:
    // Rustのロジックプロセッサへのポインタ
    LogicProcessor* logic_processor;

public:
    Service();
    ~Service();

    // メッセージ処理メソッド
    std::string processMessage(const std::string& message);
}; 