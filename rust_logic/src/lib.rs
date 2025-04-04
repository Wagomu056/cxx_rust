use std::ffi::{CStr, CString};
use std::os::raw::{c_char};
use libc;

// C++から呼び出される関数
#[no_mangle]
pub extern "C" fn process_logic(input: *const c_char) -> *mut c_char {
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
    
    // ロジック処理（例：文字列を加工）
    let processed = format!("{} (processed by Rust logic)", input_str);
    println!("Rust logic: Processing message: {}", input_str);
    
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