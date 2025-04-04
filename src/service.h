#pragma once

#include <string>
#include <functional>
#include "network.h"

// 前方宣言（Rustの構造体用）
typedef struct LogicProcessor LogicProcessor;

// サービスインターフェース
class Service {
private:
    // Rustのロジックプロセッサへのポインタ
    LogicProcessor* logic_processor;
    
    // ネットワーク処理インスタンス
    Network* network;

public:
    Service();
    ~Service();

    // コールバック用の結果型
    using ResultCallback = std::function<void(const std::string&)>;
    
    // メッセージ処理メソッド（コールバック関数を利用）
    void processMessage(const std::string& message, ResultCallback callback);
    
    // Networkインスタンスを取得（Rust側からアクセス用）
    Network* getNetwork() const { return network; }
}; 