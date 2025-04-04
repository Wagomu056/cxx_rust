use std::ffi::{CStr, CString};
use std::os::raw::c_char;

use crate::logic::LogicProcessor;
use crate::network::Network;

//---------------------------------------------------------------------
// C++に公開するFFI関数
//---------------------------------------------------------------------

/// LogicProcessorのインスタンスを作成
#[no_mangle]
pub extern "C" fn logic_processor_new() -> *mut LogicProcessor {
    let processor = Box::new(LogicProcessor::new());
    Box::into_raw(processor)
}

/// LogicProcessorのインスタンスを破棄
#[no_mangle]
pub extern "C" fn logic_processor_free(ptr: *mut LogicProcessor) {
    if !ptr.is_null() {
        unsafe {
            let _ = Box::from_raw(ptr);
        }
    }
}

/// LogicProcessorを使ってメッセージを処理し、Networkを通じて送信
#[no_mangle]
pub extern "C" fn logic_processor_process(
    processor: *const LogicProcessor, 
    network: *mut Network,
    input: *const c_char
) -> *mut c_char {
    let processor = unsafe {
        if processor.is_null() {
            return CString::new("Null processor").unwrap().into_raw();
        }
        &*processor
    };

    // C文字列をRustの文字列に変換
    let c_str = unsafe {
        if input.is_null() {
            return CString::new("Null input").unwrap().into_raw();
        }
        CStr::from_ptr(input)
    };
    
    let input_str = match c_str.to_str() {
        Ok(s) => s,
        Err(_) => return CString::new("Invalid UTF-8").unwrap().into_raw(),
    };
    
    // LogicProcessor構造体を使用してロジック処理を実行
    let processed = processor.process(input_str);
    
    // Networkを使用して送信
    let result = match processor.send_via_network(network, &processed) {
        Ok(response) => response,
        Err(err) => format!("Error: {}", err),
    };
    
    // 結果をC文字列に変換してポインタを返す
    CString::new(result).unwrap().into_raw()
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