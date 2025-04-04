// ===============================================================================
// FFI関数の実装
// 
// このモジュールはC++から呼び出し可能なFFI関数を実装します
// ===============================================================================

use std::ffi::CStr;
use std::os::raw::{c_char, c_void};
use super::logic::LogicProcessor;
use super::response::Response;

/// 新しいLogicProcessorインスタンスを作成します
#[no_mangle]
pub extern "C" fn logic_processor_new() -> *mut c_void {
    let processor = LogicProcessor::new();
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

/// メッセージを処理して結果を返します
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

/// レスポンス構造体のメモリを解放する
/// これはC++から呼び出されます
#[no_mangle]
pub extern "C" fn free_response(response: *mut Response) {
    if !response.is_null() {
        unsafe {
            let response = &mut *response;
            response.free_message();
        }
    }
} 