cmake_minimum_required(VERSION 3.10)

project(cxx_rust_ffi)

# C++の標準を17に設定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Rustのクレートに関する情報
set(RUST_CRATE_DIR ${CMAKE_SOURCE_DIR}/rust_logic)
set(RUST_TARGET_DIR ${RUST_CRATE_DIR}/target)
set(RUST_RELEASE_TARGET ${RUST_TARGET_DIR}/release)
set(RUST_LIBRUST_PATH ${RUST_RELEASE_TARGET}/librust_logic.a)

# メッセージを表示
message(STATUS "Rust library path: ${RUST_LIBRUST_PATH}")

# メイン実行ファイルのターゲットを追加
add_executable(cxx_rust_ffi src/main.cpp src/service.cpp)

# テスト実行ファイルのターゲットを追加
add_executable(service_test src/test_service.cpp src/service.cpp)

# Rustライブラリを含む
target_link_libraries(cxx_rust_ffi ${RUST_LIBRUST_PATH} dl pthread)
target_link_libraries(service_test ${RUST_LIBRUST_PATH} dl pthread)

# テストを追加
enable_testing()
add_test(NAME ServiceMessageLoggingTest COMMAND service_test) 