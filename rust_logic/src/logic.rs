// ===============================================================================
// ロジック処理の実装
// 
// このモジュールはビジネスロジックの実装を担当します
// ===============================================================================

use std::fmt;

/// ロジック処理を担当するメインの構造体
pub struct LogicProcessor {
    messages_processed: usize,
}

impl LogicProcessor {
    /// 新しいLogicProcessorインスタンスを作成
    pub fn new() -> Self {
        LogicProcessor {
            messages_processed: 0,
        }
    }
    
    /// メッセージを処理して結果を返す
    pub fn process_message(&mut self, message: &str) -> String {
        // メッセージの処理カウントを増やす
        self.messages_processed += 1;
        
        // 簡単なメッセージ処理のロジック
        let processed = format!(
            "処理結果: '{}' (これまでに処理したメッセージ数: {})",
            message, 
            self.messages_processed
        );
        
        processed
    }
}

impl fmt::Debug for LogicProcessor {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("LogicProcessor")
            .field("messages_processed", &self.messages_processed)
            .finish()
    }
} 