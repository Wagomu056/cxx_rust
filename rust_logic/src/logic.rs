// ===============================================================================
// ロジック処理の実装
// 
// このモジュールはビジネスロジックの実装を担当します
// ===============================================================================

use std::fmt;
use super::network::Network;
use super::response::Response;

/// ロジック処理を担当するメインの構造体
pub struct LogicProcessor {
    messages_processed: usize,
    network: *mut Network,  // C++から渡されるNetworkインスタンスへのポインタ
}

impl LogicProcessor {
    /// 新しいLogicProcessorインスタンスを作成
    pub fn new(network: *mut Network) -> Self {
        LogicProcessor {
            messages_processed: 0,
            network,
        }
    }
    
    /// メッセージを処理してレスポンスを返す
    /// 内部でNetworkを使用してメッセージを送信する
    pub fn process_message(&mut self, message: &str) -> Response {
        // メッセージの処理カウントを増やす
        self.messages_processed += 1;
        
        // ネットワーク経由で送信するメッセージを構築
        let formatted_message = format!(
            "[LogicProcessor] ネットワーク送信: '{}' (処理回数: {})", 
            message, 
            self.messages_processed
        );
        
        println!("LogicProcessor: Networkを使用してメッセージを送信します");
        
        // ネットワーク経由でメッセージを送信
        if self.network.is_null() {
            println!("LogicProcessor: Networkポインタがnullです！代替レスポンスを返します");
            // Networkが設定されていない場合は代替レスポンスを返す
            return Response::success(200, &format!(
                "Network未使用 - 処理結果: '{}' (これまでに処理したメッセージ数: {})",
                message, 
                self.messages_processed
            ));
        }
        
        // ネットワーク経由でメッセージを送信
        let network_result = unsafe {
            let network = &mut *self.network;
            network.send(&formatted_message)
        };
        
        // 送信結果に基づいてレスポンスを作成
        match network_result {
            Ok(response) => response,
            Err(err) => {
                println!("LogicProcessor: ネットワークエラー: {}", err);
                Response::error(500, &format!("ネットワークエラー: {}", err))
            },
        }
    }
}

impl fmt::Debug for LogicProcessor {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("LogicProcessor")
            .field("messages_processed", &self.messages_processed)
            .field("network", &self.network)
            .finish()
    }
} 