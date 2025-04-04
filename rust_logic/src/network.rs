use std::ffi::{CStr, CString};
use std::os::raw::c_char;

/// C++のNetworkクラスに対するFFI
#[repr(C)]
pub struct Network {
    _private: [u8; 0],  // opaque型としてNetworkを扱う
}

/// C++から提供されるNetwork関連の関数
extern "C" {
    pub fn network_send(network: *mut Network, message: *const c_char) -> *const c_char;
    pub fn free_network_string(ptr: *mut c_char);
}

/// Networkに関連するユーティリティ関数
impl Network {
    /// ネットワーク経由でメッセージを送信する
    pub fn send(&mut self, message: &str) -> Result<String, String> {
        if message.is_empty() {
            return Err("Empty message".to_string());
        }
        
        // メッセージをC文字列に変換
        let c_message = match CString::new(message) {
            Ok(s) => s,
            Err(_) => return Err("Failed to convert message to C string".to_string()),
        };
        
        // C++のnetwork_send関数を呼び出す
        let result_ptr = unsafe { network_send(self, c_message.as_ptr()) };
        
        if result_ptr.is_null() {
            return Err("Network send returned null".to_string());
        }
        
        // 結果をRustの文字列に変換
        let result_str = unsafe {
            let c_str = CStr::from_ptr(result_ptr);
            let rust_str = match c_str.to_str() {
                Ok(s) => s.to_string(),
                Err(_) => {
                    // エラーでも、メモリは解放する必要がある
                    free_network_string(result_ptr as *mut c_char);
                    return Err("Invalid UTF-8 in network response".to_string());
                }
            };
            
            // C++側で確保したメモリを解放
            free_network_string(result_ptr as *mut c_char);
            rust_str
        };
        
        Ok(result_str)
    }
} 