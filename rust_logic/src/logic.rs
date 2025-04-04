use crate::network::Network;

/// ロジック処理を行う構造体
pub struct LogicProcessor {
    /// メッセージに追加するプレフィックス
    prefix: String,
}

impl LogicProcessor {
    /// 新しいLogicProcessorインスタンスを作成
    pub fn new() -> Self {
        LogicProcessor {
            prefix: "(processed by Rust logic)".to_string(),
        }
    }

    /// メッセージを処理するメソッド
    pub fn process(&self, input: &str) -> String {
        println!("Rust logic: Processing message: {}", input);
        format!("{} {}", input, self.prefix)
    }
    
    /// C++のNetworkを使用してメッセージを送信するメソッド
    pub fn send_via_network(&self, network: *mut Network, message: &str) -> Result<String, String> {
        if network.is_null() {
            return Err("Network pointer is null".to_string());
        }
        
        // Networkをmutで参照
        let network = unsafe { &mut *network };
        
        // Networkのsendメソッドを使用
        network.send(message)
    }
} 