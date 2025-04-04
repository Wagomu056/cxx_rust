#pragma once

#include <string>

// ネットワーク処理クラス
class Network {
public:
    Network();
    ~Network();

    // メッセージ送信メソッド（モック）
    std::string send(const std::string& message);
}; 