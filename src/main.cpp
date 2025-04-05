#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
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
    
    std::cout << "\n--- processMessage テスト（同期処理・コールバックあり） ---" << std::endl;
    // メッセージを処理
    service.processMessage("こんにちは、世界！", handleResponse);
    
    std::cout << "\n--- send テスト（同期処理・結果を受け取らない） ---" << std::endl;
    // メッセージを送信するだけ（結果は受け取らない）
    service.send("送信のみのメッセージ");
    
    std::cout << "\n--- sendToQueue テスト（非同期処理・キュー使用） ---" << std::endl;
    // メッセージをキューに追加
    for (int i = 1; i <= 5; i++) {
        service.sendToQueue("キューメッセージ #" + std::to_string(i));
        
        // メッセージ間に少し待機
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // キューの処理を待つ
    std::cout << "\nキューのバックグラウンド処理中..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "キューの処理が完了したと想定（結果は確認しません）" << std::endl;
    
    std::cout << "\n--- 最後の同期メッセージ ---" << std::endl;
    service.processMessage("最後のメッセージ", handleResponse);
    
    std::cout << "\nプログラム正常終了" << std::endl;
    return 0;
} 