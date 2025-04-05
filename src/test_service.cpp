#include "service.h"
#include "response.h"
#include "network.h"
#include <iostream>
#include <cassert>
#include <string>
#include <functional>
#include <vector>

// メッセージ処理とResponseの構造が正しく機能することをテストする関数
bool test_response_handling() {
    // テスト用のメッセージ
    const std::string test_message = "テストメッセージ";
    
    // 結果を格納する変数
    bool callback_called = false;
    bool response_success = false;
    int response_code = 0;
    std::string response_message;
    
    // テスト前にメッセージログをクリア
    Network::clearMessageLog();
    
    // Serviceを作成して処理を実行
    Service service;
    service.processMessage(test_message, [&](const Response& response) {
        callback_called = true;
        response_success = response.success;
        response_code = response.response_code;
        response_message = response.message;
        
        std::cout << "コールバック受信: " 
                  << "成功=" << (response.success ? "はい" : "いいえ")
                  << ", コード=" << response.response_code
                  << ", メッセージ='" << response.message << "'" << std::endl;
    });
    
    // コールバックが呼ばれたか確認
    if (!callback_called) {
        std::cerr << "エラー: コールバックが呼び出されませんでした" << std::endl;
        return false;
    }
    
    // レスポンスの内容を検証
    if (!response_success) {
        std::cerr << "エラー: レスポンスが成功ではありません" << std::endl;
        return false;
    }
    
    if (response_code != 200) {
        std::cerr << "エラー: レスポンスコードが期待値（200）ではありません: " 
                  << response_code << std::endl;
        return false;
    }
    
    // メッセージの内容を検証
    if (response_message.find(test_message) == std::string::npos) {
        std::cerr << "エラー: レスポンスメッセージに元のメッセージが含まれていません: " 
                  << response_message << std::endl;
        return false;
    }
    
    std::cout << "テスト成功: レスポンス構造が正しく処理されました" << std::endl;
    return true;
}

// send関数が正しくメッセージを送信することをテストする関数
bool test_send_function() {
    std::cout << "\n--- send関数のテスト開始 ---" << std::endl;
    
    // テスト用のメッセージ
    const std::string test_message1 = "送信テストメッセージ1";
    const std::string test_message2 = "送信テストメッセージ2";
    
    // テスト前にメッセージログをクリア
    Network::clearMessageLog();
    
    // Serviceを作成して送信を実行
    Service service;
    service.send(test_message1);
    service.send(test_message2);
    
    // Networkが受信したメッセージのログを取得
    auto message_log = Network::getMessageLog();
    
    // メッセージログの長さを検証
    if (message_log.size() < 2) {
        std::cerr << "エラー: 期待される数のメッセージが送信されていません。送信数: " 
                  << message_log.size() << ", 期待: 2以上" << std::endl;
        return false;
    }
    
    // 最後の2つのメッセージをチェック
    bool found_message1 = false;
    bool found_message2 = false;
    
    // ログをループして各メッセージを確認
    for (const auto& message : message_log) {
        if (message.find(test_message1) != std::string::npos) {
            found_message1 = true;
            std::cout << "メッセージ1が見つかりました: " << message << std::endl;
        }
        if (message.find(test_message2) != std::string::npos) {
            found_message2 = true;
            std::cout << "メッセージ2が見つかりました: " << message << std::endl;
        }
    }
    
    // 両方のメッセージが見つかったか確認
    if (!found_message1) {
        std::cerr << "エラー: メッセージ1 '" << test_message1 << "' がログに見つかりません" << std::endl;
        return false;
    }
    
    if (!found_message2) {
        std::cerr << "エラー: メッセージ2 '" << test_message2 << "' がログに見つかりません" << std::endl;
        return false;
    }
    
    std::cout << "テスト成功: send関数が正しくメッセージを送信しました" << std::endl;
    return true;
}

// ユニットテストのメイン関数
int main() {
    bool response_test_passed = test_response_handling();
    bool send_test_passed = test_send_function();
    
    if (response_test_passed && send_test_passed) {
        std::cout << "\n全てのテストが成功しました！" << std::endl;
        return 0; // テスト成功
    } else {
        std::cerr << "\nテストに失敗しました。" << std::endl;
        if (!response_test_passed) std::cerr << "- Response処理テストが失敗" << std::endl;
        if (!send_test_passed) std::cerr << "- Send関数テストが失敗" << std::endl;
        return 1; // テスト失敗
    }
} 