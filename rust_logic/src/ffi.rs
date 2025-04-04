use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_void};

use crate::logic::LogicProcessor;
use crate::network::Network;
use crate::response::{Response, free_message_ptr};

//---------------------------------------------------------------------
// C++に公開するFFI関数
//---------------------------------------------------------------------

/// LogicProcessorのインスタンスを作成
#[no_mangle]
pub extern "C" fn logic_processor_new() -> *mut c_void {
    let processor = LogicProcessor::new();
    let boxed = Box::new(processor);
    Box::into_raw(boxed) as *mut c_void
}

/// LogicProcessorのインスタンスを破棄
#[no_mangle]
pub extern "C" fn logic_processor_free(ptr: *mut c_void) {
    if ptr.is_null() {
        return;
    }
    
    unsafe {
        let _ = Box::from_raw(ptr as *mut LogicProcessor);
    }
}

/// LogicProcessorを使ってメッセージを処理し、Networkを通じて送信
#[no_mangle]
pub extern "C" fn logic_processor_process(ptr: *mut c_void, message: *const c_char) -> Response {
    let result = unsafe {
        if ptr.is_null() || message.is_null() {
            return Response::error(400, "無効なポインタが渡されました");
        }
        
        let processor = &mut *(ptr as *mut LogicProcessor);
        let c_str = CStr::from_ptr(message);
        
        match c_str.to_str() {
            Ok(msg) => {
                let response = processor.process_message(msg);
                Response::success(200, &response)
            },
            Err(_) => {
                Response::error(400, "無効なUTF-8文字列です")
            }
        }
    };

    result
}

/// Responseのメッセージフィールドのメモリを解放します
#[no_mangle]
pub extern "C" fn free_response_message(message: *mut c_char) {
    free_message_ptr(message);
}

/// Rustが確保したメモリを解放する
#[no_mangle]
pub extern "C" fn free_rust_string(ptr: *mut c_char) {
    unsafe {
        if !ptr.is_null() {
            // ポインタをCStringに変換し、Rustのメモリ管理に戻す
            let _ = CString::from_raw(ptr);
        }
    }
} 