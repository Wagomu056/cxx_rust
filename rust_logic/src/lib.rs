use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_void};
use libc;

// C++のNetworkクラスに対するFFI
#[repr(C)]
pub struct Network {
    _private: [u8; 0],  // opaque型としてNetworkを扱う
}

// C++から提供されるNetwork関連の関数
extern "C" {
    fn network_send(network: *mut Network, message: *const c_char) -> *const c_char;
    fn free_network_string(ptr: *mut c_char);
}

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
    
    // C++のNetworkを使用してメッセージを送信するメソッド
    fn send_via_network(&self, network: *mut Network, message: &str) -> Result<String, String> {
        if network.is_null() {
            return Err("Network pointer is null".to_string());
        }
        
        // メッセージをC文字列に変換
        let c_message = match CString::new(message) {
            Ok(s) => s,
            Err(_) => return Err("Failed to convert message to C string".to_string()),
        };
        
        // C++のnetwork_send関数を呼び出す
        let result_ptr = unsafe { network_send(network, c_message.as_ptr()) };
        
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

// C++から呼び出される処理関数（Networkポインタを受け取る）
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
