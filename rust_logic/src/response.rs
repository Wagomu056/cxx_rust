use std::ffi::CString;
use std::os::raw::c_char;

/// C++に返すレスポンス構造体
#[repr(C)]
pub struct Response {
    /// 成功/失敗のフラグ
    pub success: bool,
    /// レスポンスコード
    pub response_code: i32,
    /// レスポンスメッセージ（C文字列）
    pub message: *mut c_char,
}

impl Response {
    /// 成功レスポンスを作成
    pub fn success(code: i32, message: &str) -> Self {
        let c_str = CString::new(message).unwrap();
        Response {
            success: true,
            response_code: code,
            message: c_str.into_raw(),
        }
    }
    
    /// エラーレスポンスを作成
    pub fn error(code: i32, message: &str) -> Self {
        let c_str = CString::new(message).unwrap();
        Response {
            success: false,
            response_code: code,
            message: c_str.into_raw(),
        }
    }
}

/// レスポンスメッセージのメモリを解放する内部実装
pub fn free_message_ptr(message: *mut c_char) {
    if !message.is_null() {
        unsafe {
            let _ = CString::from_raw(message);
        }
    }
} 