cmake_minimum_required(VERSION 3.10)

project(cxx_rust_ffi)

# C++の標準を17に設定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 常にリリースビルドを強制
set(CMAKE_BUILD_TYPE Release)

# Rustのクレートに関する情報
set(RUST_CRATE_DIR ${CMAKE_SOURCE_DIR}/rust_logic)
set(RUST_TARGET_DIR ${RUST_CRATE_DIR}/target)

# Set cargo executable path directly
set(CARGO_EXECUTABLE "/Users/takuya/.cargo/bin/cargo")
if(NOT EXISTS ${CARGO_EXECUTABLE})
    message(FATAL_ERROR "Cargo executable not found at ${CARGO_EXECUTABLE}. Please check the path.")
endif()

# Releaseビルド用の設定
set(CARGO_BUILD_ARGS "build" "--release")
set(RUST_LIBRARY_PATH ${RUST_TARGET_DIR}/release/librust_logic.a)

message(STATUS "Rust library path: ${RUST_LIBRARY_PATH}")
message(STATUS "Cargo build command: ${CARGO_EXECUTABLE} ${CARGO_BUILD_ARGS}")

# RustのCrateをビルドするカスタムコマンド
add_custom_command(
    OUTPUT ${RUST_LIBRARY_PATH}
    COMMAND ${CARGO_EXECUTABLE} ${CARGO_BUILD_ARGS}
    WORKING_DIRECTORY ${RUST_CRATE_DIR}
    COMMENT "Building Rust library..."
    VERBATIM
)

# Rustビルドをターゲットとして追加
add_custom_target(rust_build DEPENDS ${RUST_LIBRARY_PATH})

# メイン実行ファイルのターゲットを追加
add_executable(cxx_rust_ffi src/main.cpp src/service.cpp src/network.cpp)
add_executable(service_test src/test_service.cpp src/service.cpp src/network.cpp)
add_dependencies(cxx_rust_ffi rust_build)
add_dependencies(service_test rust_build)

# Rustライブラリを含む
target_link_libraries(cxx_rust_ffi ${RUST_LIBRARY_PATH})
target_link_libraries(service_test ${RUST_LIBRARY_PATH})

# テストを追加
enable_testing()
add_test(NAME ServiceMessageLoggingTest COMMAND service_test) 