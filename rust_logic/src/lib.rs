// ===============================================================================
// Rust FFIライブラリ
// 
// このモジュールはRustによるビジネスロジックとC++のFFI連携を実装します
// ===============================================================================

mod network;    // C++のネットワークコードへのFFIインターフェース
mod response;   // C++とRust間で共有されるレスポンス構造体
mod logic;      // ビジネスロジック実装
mod ffi;        // C++に公開するFFI関数

// 必要なモジュールを公開
pub use network::Network;
pub use response::Response;
pub use logic::LogicProcessor; 