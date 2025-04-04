use std::ffi::{CStr, CString};
use std::os::raw::{c_char};
use libc;

// ロジック処理を行う構造体
pub struct LogicProcessor {
    // 必要に応じてフィールドを追加
    prefix: String,
}

impl LogicProcessor {
    // 新しいLogicProcessorインスタンスを作成
    fn new() -> Self {
        LogicProcessor {
            prefix: "(processed by Rust logic)".to_string(),
        }
    }

    // メッセージを処理するメソッド
    fn process(&self, input: &str) -> String {
        println!("Rust logic: Processing message: {}", input);
        format!("{} {}", input, self.prefix)
    }
}

// C++から呼び出されるコンストラクタ関数
#[no_mangle]
pub extern "C" fn logic_processor_new() -> *mut LogicProcessor {
    let processor = Box::new(LogicProcessor::new());
    Box::into_raw(processor)
}

// C++から呼び出されるデストラクタ関数
#[no_mangle]
pub extern "C" fn logic_processor_free(ptr: *mut LogicProcessor) {
    if !ptr.is_null() {
        unsafe {
            let _ = Box::from_raw(ptr);
        }
    }
}

// C++から呼び出される処理関数
#[no_mangle]
pub extern "C" fn logic_processor_process(processor: *const LogicProcessor, input: *const c_char) -> *mut c_char {
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
    
    // 結果をC文字列に変換してポインタを返す
    CString::new(processed).unwrap().into_raw()
}

// C++側でRustが確保したメモリを解放するための関数
#[no_mangle]
pub extern "C" fn free_rust_string(ptr: *mut c_char) {
    unsafe {
        if !ptr.is_null() {
            // ポインタをCStringに変換し、Rustのメモリ管理に戻す
            let _ = CString::from_raw(ptr);
        }
    }
} 
