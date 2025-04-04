#include "service.h"
#include "response.h"
#include <iostream>
#include <cassert>
#include <string>
#include <functional>

// メッセージ処理とResponseの構造が正しく機能することをテストする関数
bool test_response_handling() {
    // テスト用のメッセージ
    const std::string test_message = "テストメッセージ";
    
    // 結果を格納する変数
    bool callback_called = false;
    bool response_success = false;
    int response_code = 0;
    std::string response_message;
    
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

// ユニットテストのメイン関数
int main() {
    if (test_response_handling()) {
        return 0; // テスト成功
    } else {
        return 1; // テスト失敗
    }
} 