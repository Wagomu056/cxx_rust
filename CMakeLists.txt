cmake_minimum_required(VERSION 3.10)
project(CxxRustFFI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CTestを有効化
enable_testing()

# Rustライブラリのパス（ビルド後に設定する必要があります）
set(RUST_LIB_DIR ${CMAKE_SOURCE_DIR}/rust_logic/target/release)

# 共通のソースファイル（mainを除く）
set(COMMON_SOURCES
    src/service.cpp
    src/network.cpp
)

# メイン実行ファイル
add_executable(cxx_rust_ffi src/main.cpp ${COMMON_SOURCES})

# インクルードディレクトリ
target_include_directories(cxx_rust_ffi PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Rustライブラリをリンク
target_link_libraries(cxx_rust_ffi PRIVATE
    ${RUST_LIB_DIR}/librust_logic.a
)

# ユニットテスト実行ファイル
add_executable(test_service src/test_service.cpp ${COMMON_SOURCES})
target_include_directories(test_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_service PRIVATE
    ${RUST_LIB_DIR}/librust_logic.a
)

# テストを追加
add_test(
    NAME ServiceMessageLoggingTest
    COMMAND test_service
)

# Rustライブラリのビルドコマンド（両方の対象で共有）
add_custom_command(TARGET cxx_rust_ffi PRE_BUILD
    COMMAND cd ${CMAKE_SOURCE_DIR}/rust_logic && cargo build --release
    COMMENT "Building Rust library..."
)

add_custom_command(TARGET test_service PRE_BUILD
    COMMAND cd ${CMAKE_SOURCE_DIR}/rust_logic && cargo build --release
    COMMENT "Building Rust library for tests..."
) 