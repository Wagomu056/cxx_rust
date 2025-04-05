use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use std::sync::Mutex;
use std::collections::HashMap;
use std::sync::atomic::{AtomicU64, Ordering};
use super::response::Response;

/// C++のNetworkクラスに対するFFI
#[repr(C)]
pub struct Network {
    _private: [u8; 0],  // opaque型としてNetworkを扱う
}

/// C++のコールバック関数型
pub type NetworkResponseCallback = extern "C" fn(u64, Response);

// C++から提供されるNetwork関連の関数
extern "C" {
    pub fn network_send(network: *mut Network, message: *const c_char, callback_id: u64, callback: NetworkResponseCallback);
}

// コールバック管理
lazy_static::lazy_static! {
    static ref CALLBACK_MAP: Mutex<HashMap<u64, Box<dyn FnOnce(Response) + Send + 'static>>> = Mutex::new(HashMap::new());
    static ref CALLBACK_ID: AtomicU64 = AtomicU64::new(1);
}

// C++側から呼び出されるコールバック
#[no_mangle]
pub extern "C" fn rust_network_callback(id: u64, response: Response) {
    // コールバックマップからコールバック関数を取得して実行
    if let Some(callback) = CALLBACK_MAP.lock().unwrap().remove(&id) {
        callback(response);
    }
}

/// Networkに関連するユーティリティ関数
impl Network {
    /// ネットワーク経由でメッセージを送信する
    /// コールバック関数を受け取り、結果が準備できたらコールバックを呼び出す
    pub fn send<F>(&mut self, message: &str, callback: F) 
    where 
        F: FnOnce(Result<Response, String>) + Send + 'static
    {
        if message.is_empty() {
            callback(Err("Empty message".to_string()));
            return;
        }
        
        // メッセージをC文字列に変換
        let c_message = match CString::new(message) {
            Ok(s) => s,
            Err(_) => {
                callback(Err("Failed to convert message to C string".to_string()));
                return;
            }
        };
        
        // ユニークなコールバックIDを生成
        let callback_id = CALLBACK_ID.fetch_add(1, Ordering::SeqCst);
        
        // コールバックを変換してグローバルマップに保存
        let wrapped_callback = Box::new(move |response: Response| {
            if response.success {
                callback(Ok(response));
            } else {
                // エラーメッセージを抽出
                let error_message = if !response.message.is_null() {
                    unsafe {
                        let c_str = CStr::from_ptr(response.message);
                        match c_str.to_str() {
                            Ok(s) => s.to_string(),
                            Err(_) => "Invalid UTF-8 in error message".to_string(),
                        }
                    }
                } else {
                    format!("Network error with code: {}", response.response_code)
                };
                
                callback(Err(error_message));
            }
        });
        
        // グローバルマップにコールバックを登録
        CALLBACK_MAP.lock().unwrap().insert(callback_id, wrapped_callback);
        
        // C++側のnetwork_send関数を呼び出す
        unsafe { 
            network_send(self, c_message.as_ptr(), callback_id, rust_network_callback);
        }
    }
} 