// ===============================================================================
// Rust Logic FFI ライブラリ
// 
// このファイルはRustライブラリのエントリーポイントです。
// C++からはこのモジュールを通じて以下の関数が利用可能です：
//
// - logic_processor_new()
// - logic_processor_free()
// - logic_processor_process()
// - free_response_message()
// ===============================================================================

// モジュール定義
mod network;  // C++のNetworkクラスとのインターフェース
mod logic;    // ロジック処理の実装
mod ffi;      // C++に公開するFFI関数
mod response; // レスポンス構造体の定義

// 公開する型と関数
pub use network::Network;
pub use logic::LogicProcessor;
pub use response::Response;
pub use ffi::*; 