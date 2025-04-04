#include "service.h"
#include "network.h"
#include <iostream>
#include <cassert>
#include <string>
#include <functional>

// メッセージ処理が正しくログに記録されることをテストする関数
bool test_message_logging() {
    // テスト前にメッセージログをクリア
    Network::clearMessageLog();
    
    // テスト用のメッセージ
    const std::string test_message = "Test message from unit test";
    
    // 結果を格納する変数
    std::string result;
    bool callback_called = false;
    
    // Serviceを作成して処理を実行（コールバック版）
    Service service;
    service.processMessage(test_message, [&result, &callback_called](const std::string& response) {
        result = response;
        callback_called = true;
        std::cout << "Callback received: " << response << std::endl;
    });
    
    // コールバックが呼ばれたか確認
    if (!callback_called) {
        std::cerr << "Error: Callback was not called" << std::endl;
        return false;
    }
    
    // ログを取得
    std::vector<std::string> log = Network::getMessageLog();
    
    // 検証: ログにメッセージが1つだけ記録されていること
    if (log.size() != 1) {
        std::cerr << "Error: Expected 1 log entry, but got " << log.size() << std::endl;
        return false;
    }
    
    // 検証: ログの内容が期待通りであること (Rustの処理が適用されたもの)
    std::string expected_content = test_message + " (processed by Rust logic)";
    if (log[0] != expected_content) {
        std::cerr << "Error: Expected log content '" << expected_content 
                  << "', but got '" << log[0] << "'" << std::endl;
        return false;
    }
    
    // 検証: コールバックで受け取った結果の内容
    if (result.find("Response:") == std::string::npos || 
        result.find(test_message) == std::string::npos ||
        result.find("processed by Rust logic") == std::string::npos) {
        std::cerr << "Error: Unexpected result content: " << result << std::endl;
        return false;
    }
    
    std::cout << "Test passed: Message was correctly logged and callback was called" << std::endl;
    return true;
}

// ユニットテストのメイン関数
int main() {
    if (test_message_logging()) {
        return 0; // テスト成功
    } else {
        return 1; // テスト失敗
    }
} 