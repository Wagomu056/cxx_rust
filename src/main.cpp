#include <iostream>
#include "service.h"
#include "response.h"

// レスポンス処理用のコールバック関数
void handleResponse(const Response& response) {
    std::cout << "=== レスポンス受信 ===" << std::endl;
    std::cout << "成功: " << (response.success ? "はい" : "いいえ") << std::endl;
    std::cout << "コード: " << response.response_code << std::endl;
    std::cout << "メッセージ: " << response.message << std::endl;
    std::cout << "===================" << std::endl;
}

int main() {
    std::cout << "C++/Rust FFI テストプログラム開始" << std::endl;
    
    // サービスインスタンスを作成
    Service service;
    
    std::cout << "\n--- processMessage テスト（コールバックあり） ---" << std::endl;
    // メッセージを処理
    service.processMessage("こんにちは、世界！", handleResponse);
    service.processMessage("テストメッセージ", handleResponse);
    
    std::cout << "\n--- send テスト（結果を受け取らない） ---" << std::endl;
    // メッセージを送信するだけ（結果は受け取らない）
    service.send("送信のみのメッセージ1");
    service.send("送信のみのメッセージ2");
    
    std::cout << "\n--- 最後のprocessMessage ---" << std::endl;
    service.processMessage("最後のメッセージ", handleResponse);
    
    std::cout << "\nプログラム正常終了" << std::endl;
    return 0;
} 