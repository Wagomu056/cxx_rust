// ===============================================================================
// ロジック処理の実装
// 
// このモジュールはビジネスロジックの実装を担当します
// ===============================================================================

use std::fmt;
use std::thread;
use std::sync::{Arc, Mutex, atomic::{AtomicBool, Ordering}};
use std::time::Duration;
use crossbeam_queue::SegQueue;
use super::network::Network;

/// キューに格納するメッセージとメタデータを表す構造体
//@todo Colneが必要かを確認
#[derive(Clone)]
struct QueuedMessage {
    content: String,
    id: usize,
}

// 生ポインタをラップして送信可能にするための構造体
struct NetworkSendPtr(*mut Network);
// 安全ではないがC++からのポインタを扱うために必要
unsafe impl Send for NetworkSendPtr {}
unsafe impl Sync for NetworkSendPtr {}

/// ロジック処理を担当するメインの構造体
pub struct LogicProcessor {
    messages_processed: Arc<Mutex<usize>>,
    next_message_id: Arc<Mutex<usize>>,
    network: Arc<Mutex<NetworkSendPtr>>,  // C++から渡されるNetworkインスタンスへのポインタ
    message_queue: Arc<SegQueue<QueuedMessage>>,  // 処理待ちメッセージのキュー
    worker_running: Arc<AtomicBool>,              // ワーカースレッドの状態フラグ
}

// FFIでポインタを共有するため、SendとSyncを実装
unsafe impl Send for LogicProcessor {}
unsafe impl Sync for LogicProcessor {}

impl LogicProcessor {
    /// 新しいLogicProcessorインスタンスを作成
    pub fn new(network: *mut Network) -> Self {
        println!("LogicProcessor: 新しいLogicProcessorインスタンスを作成");
        let messages_processed = Arc::new(Mutex::new(0));
        let next_message_id = Arc::new(Mutex::new(1));
        let message_queue = Arc::new(SegQueue::new());
        let worker_running = Arc::new(AtomicBool::new(true));
        let network_ptr = Arc::new(Mutex::new(NetworkSendPtr(network)));
        
        let processor = LogicProcessor {
            messages_processed,
            next_message_id,
            network: network_ptr,
            message_queue: Arc::clone(&message_queue),
            worker_running: Arc::clone(&worker_running),
        };
        
        // バックグラウンドワーカースレッドを開始
        processor.start_worker_thread();
        
        processor
    }
    
    /// バックグラウンドワーカースレッドを起動
    fn start_worker_thread(&self) {
        println!("LogicProcessor: バックグラウンドワーカースレッド起動");
        let message_queue = Arc::clone(&self.message_queue);
        let messages_processed = Arc::clone(&self.messages_processed);
        let worker_running = Arc::clone(&self.worker_running);
        let network_ptr = Arc::clone(&self.network);
        
        thread::spawn(move || {
            println!("LogicProcessor: メッセージ処理ワーカースレッド開始");
            
            while worker_running.load(Ordering::SeqCst) {
                // キューからメッセージを取得（非ブロッキング）
                match message_queue.pop() {
                    Some(queued_msg) => {
                        // メッセージの処理カウントを増やす
                        let mut count = messages_processed.lock().unwrap();
                        *count += 1;
                        let current_count = *count;
                        drop(count);  // ロックを解放
                        
                        println!("LogicProcessor [worker]: メッセージをキューから取得: '{}' (ID: {})", 
                                 queued_msg.content, queued_msg.id);
                        
                        // ネットワークポインタを取得
                        let raw_network_ptr = {
                            let guard = network_ptr.lock().unwrap();
                            guard.0
                        };
                        
                        // ネットワークポインタがあれば、実際にメッセージを送信する
                        if !raw_network_ptr.is_null() {
                            // メッセージを処理
                            let formatted_message = format!(
                                "[LogicProcessor] キュー処理: '{}' (ID: {}, 処理回数: {})", 
                                queued_msg.content, 
                                queued_msg.id,
                                current_count
                            );
                            
                            // ネットワーク経由でメッセージを送信
                            unsafe {
                                let network = &mut *raw_network_ptr;
                                match network.send(&formatted_message) {
                                    Ok(_) => {
                                        println!("LogicProcessor [worker]: メッセージを正常に送信しました");
                                    },
                                    Err(err) => {
                                        println!("LogicProcessor [worker]: 送信エラー: {}", err);
                                    },
                                }
                            }
                        } else {
                            println!("LogicProcessor [worker]: ネットワークポインタがnullです");
                        }
                        
                        println!("LogicProcessor [worker]: メッセージ処理完了: '{}'", queued_msg.content);
                    },
                    None => {
                        // キューが空の場合は少し待機
                        thread::sleep(Duration::from_millis(100));
                    }
                }
            }
            
            println!("LogicProcessor: メッセージ処理ワーカースレッド終了");
        });
        
        println!("LogicProcessor: バックグラウンドワーカースレッドを開始しました");
    }
    
    /// メッセージをキューに追加する（非同期処理）
    pub fn queue_message(&self, message: &str) {
        println!("LogicProcessor: メッセージをキューに追加: '{}'", message);
        
        // メッセージIDを生成
        let message_id = {
            let mut id = self.next_message_id.lock().unwrap();
            let current_id = *id;
            *id += 1;
            current_id
        };
        
        // メッセージをキューに追加
        let queued_message = QueuedMessage {
            content: message.to_string(),
            id: message_id,
        };
        self.message_queue.push(queued_message);
        
        // 現在のキューサイズを取得（概算）
        let queue_size = self.message_queue.len();
        
        println!("LogicProcessor: メッセージをキューに追加しました (ID: {}, キューサイズ: {})",
                message_id, queue_size);
    }
}

impl Drop for LogicProcessor {
    fn drop(&mut self) {
        println!("LogicProcessor: 終了処理を開始");
        // ワーカースレッドを停止するフラグを設定
        self.worker_running.store(false, Ordering::SeqCst);
        // 少し待機してワーカースレッドが停止するのを待つ
        thread::sleep(Duration::from_millis(200));
        println!("LogicProcessor: 終了処理完了");
    }
}

impl fmt::Debug for LogicProcessor {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let count = self.messages_processed.lock().unwrap();
        let network = self.network.lock().unwrap();
        
        f.debug_struct("LogicProcessor")
            .field("messages_processed", &*count)
            .field("network", &format!("{:p}", network.0))
            .field("queue_size", &self.message_queue.len())
            .finish()
    }
} 