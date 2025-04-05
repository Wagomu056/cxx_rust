// ===============================================================================
// FFI関数の実装
// 
// このモジュールはC++から呼び出し可能なFFI関数を実装します
// ===============================================================================

use std::ffi::CStr;
use std::os::raw::{c_char, c_void};
use super::logic::LogicProcessor;
use super::network::Network;

/// 新しいLogicProcessorインスタンスを作成します（Networkポインタを受け取る）
#[no_mangle]
pub extern "C" fn logic_processor_new(network: *mut Network) -> *mut c_void {
    if network.is_null() {
        // ネットワークがnullの場合、ログに記録して空のポインタを渡す
        eprintln!("Warning: Null network pointer passed to logic_processor_new");
    }
    
    let processor = LogicProcessor::new(network);
    let boxed = Box::new(processor);
    Box::into_raw(boxed) as *mut c_void
}

/// LogicProcessorインスタンスを解放します
#[no_mangle]
pub extern "C" fn logic_processor_free(ptr: *mut c_void) {
    if ptr.is_null() {
        return;
    }
    
    unsafe {
        let _ = Box::from_raw(ptr as *mut LogicProcessor);
    }
}

/// メッセージをキューに追加します（非同期処理）
#[no_mangle]
pub extern "C" fn logic_processor_queue_message(ptr: *mut c_void, message: *const c_char) {
    unsafe {
        if ptr.is_null() || message.is_null() {
            println!("logic_processor_queue_message: 無効なポインタが渡されました");
            return;
        }
        
        let processor = &*(ptr as *mut LogicProcessor);
        let c_str = CStr::from_ptr(message);
        
        match c_str.to_str() {
            Ok(msg) => {
                processor.queue_message(msg);
                println!("message queued: {}", msg);
            },
            Err(_) => {
                println!("logic_processor_queue_message: 無効なUTF-8文字列です");
            }
        }
    }
} 