#include <iostream>
#include <thread>
#include <chrono>
#include "service.h"

int main() {
    std::cout << "C++/Rust FFI テストプログラム開始" << std::endl;
    
    // サービスインスタンスを作成
    Service service;
    
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
    
    std::cout << "\nプログラム正常終了" << std::endl;
    return 0;
} 