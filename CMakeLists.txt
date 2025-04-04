cmake_minimum_required(VERSION 3.10)
project(CxxRustFFI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Rustライブラリのパス（ビルド後に設定する必要があります）
set(RUST_LIB_DIR ${CMAKE_SOURCE_DIR}/rust_logic/target/release)

# ソースファイル
set(SOURCES
    src/main.cpp
    src/service.cpp
    src/network.cpp
)

# 実行ファイル
add_executable(cxx_rust_ffi ${SOURCES})

# インクルードディレクトリ
target_include_directories(cxx_rust_ffi PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Rustライブラリをリンク
target_link_libraries(cxx_rust_ffi PRIVATE
    ${RUST_LIB_DIR}/librust_logic.a
)

# Rustライブラリのビルドコマンド
add_custom_command(TARGET cxx_rust_ffi PRE_BUILD
    COMMAND cd ${CMAKE_SOURCE_DIR}/rust_logic && cargo build --release
    COMMENT "Building Rust library..."
) 