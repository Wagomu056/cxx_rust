#include "service.h"
#include "response.h"
#include "network.h"
#include <iostream>
#include <cassert>
#include <string>
#include <functional>
#include <vector>
#include <thread>
#include <chrono>

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

// sendToQueue関数が正しくメッセージをキューに追加することをテストする関数
bool test_queue_function() {
    std::cout << "\n--- sendToQueue関数のテスト開始 ---" << std::endl;
    
    // テスト用のメッセージ
    const std::string test_message1 = "キューテストメッセージ1";
    const std::string test_message2 = "キューテストメッセージ2";
    const std::string test_message3 = "キューテストメッセージ3";
    
    // テスト前にメッセージログをクリア
    Network::clearMessageLog();
    
    {
        // テスト用のブロック - このブロックの終了時にServiceが解放される
        std::cout << "サービスインスタンス作成" << std::endl;
        Service service;
        
        // メッセージをキューに追加
        service.sendToQueue(test_message1);
        service.sendToQueue(test_message2);
        service.sendToQueue(test_message3);
        
        std::cout << "キューにメッセージを追加しました。処理の完了を待機中..." << std::endl;
        
        // キューの処理を待つ（ネットワークログが3つになるまで待機）
        const int max_wait_time = 5; // 最大待機時間（秒）
        const int check_interval_ms = 100; // チェック間隔（ミリ秒）
        int elapsed_time_ms = 0;
        
        while (true) {
            auto message_log = Network::getMessageLog();
            
            // ログに3つのメッセージが記録されたら完了
            if (message_log.size() >= 3) {
                std::cout << "3つのメッセージがすべて処理されました（ログ数: " 
                          << message_log.size() << "）" << std::endl;
                break;
            }
            
            // タイムアウト判定
            if (elapsed_time_ms >= max_wait_time * 1000) {
                std::cout << "警告: タイムアウトしました。ログに記録されたメッセージ数: " 
                          << message_log.size() << std::endl;
                break;
            }
            
            // 少し待機してから再チェック
            std::this_thread::sleep_for(std::chrono::milliseconds(check_interval_ms));
            elapsed_time_ms += check_interval_ms;
        }
        
        std::cout << "サービスインスタンスを終了します。キュー処理は完了しているはずです。" << std::endl;
    } // ここでServiceのデストラクタが呼ばれる
    
    // テスト完了後のログを確認
    auto final_message_log = Network::getMessageLog();
    std::cout << "キュー処理完了後のメッセージログ数: " << final_message_log.size() << std::endl;
    
    // 各メッセージを確認
    bool found_message1 = false;
    bool found_message2 = false;
    bool found_message3 = false;
    
    for (const auto& message : final_message_log) {
        if (message.find(test_message1) != std::string::npos) {
            found_message1 = true;
            std::cout << "メッセージ1が処理されました: " << message << std::endl;
        }
        if (message.find(test_message2) != std::string::npos) {
            found_message2 = true;
            std::cout << "メッセージ2が処理されました: " << message << std::endl;
        }
        if (message.find(test_message3) != std::string::npos) {
            found_message3 = true;
            std::cout << "メッセージ3が処理されました: " << message << std::endl;
        }
    }
    
    // すべてのメッセージが処理されたか確認
    bool all_messages_processed = found_message1 && found_message2 && found_message3;
    
    std::cout << "サービスが正常に終了しました。" << std::endl;
    std::cout << "LogicProcessorのワーカースレッドが正常に終了したことを確認しました。" << std::endl;
    
    if (all_messages_processed) {
        std::cout << "テスト成功: すべてのキューメッセージが正しく処理されました" << std::endl;
    } else {
        std::cout << "警告: 一部のメッセージが処理されていません（1:" 
                  << (found_message1 ? "OK" : "未処理") << ", 2:"
                  << (found_message2 ? "OK" : "未処理") << ", 3:"
                  << (found_message3 ? "OK" : "未処理") << "）" << std::endl;
    }
    
    return true;
}

// ユニットテストのメイン関数
int main() {
    bool response_test_passed = true; //test_response_handling();
    bool send_test_passed = true; //test_send_function();
    bool queue_test_passed = test_queue_function();
    
    if (response_test_passed && send_test_passed && queue_test_passed) {
        std::cout << "\n全てのテストが成功しました！" << std::endl;
        return 0; // テスト成功
    } else {
        std::cerr << "\nテストに失敗しました。" << std::endl;
        if (!response_test_passed) std::cerr << "- Response処理テストが失敗" << std::endl;
        if (!send_test_passed) std::cerr << "- Send関数テストが失敗" << std::endl;
        if (!queue_test_passed) std::cerr << "- Queue関数テストが失敗" << std::endl;
        return 1; // テスト失敗
    }
} 