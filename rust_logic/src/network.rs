use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use super::response::Response;

/// C++のNetworkクラスに対するFFI
#[repr(C)]
pub struct Network {
    _private: [u8; 0],  // opaque型としてNetworkを扱う
}

// C++から提供されるNetwork関連の関数
extern "C" {
    pub fn network_send(network: *mut Network, message: *const c_char) -> Response;
}

/// Networkに関連するユーティリティ関数
impl Network {
    /// ネットワーク経由でメッセージを送信する
    pub fn send(&mut self, message: &str) -> Result<Response, String> {
        if message.is_empty() {
            return Err("Empty message".to_string());
        }
        
        // メッセージをC文字列に変換
        let c_message = match CString::new(message) {
            Ok(s) => s,
            Err(_) => return Err("Failed to convert message to C string".to_string()),
        };
        
        // C++のnetwork_send関数を呼び出す
        let response = unsafe { network_send(self, c_message.as_ptr()) };
        
        // 成功した場合はResponseを返す
        if response.success {
            Ok(response)
        } else {
            // エラーの場合はメッセージをRustの文字列に変換
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
            
            Err(error_message)
        }
    }
} 