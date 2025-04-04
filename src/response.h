#pragma once

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

// Rustから返されるレスポンス構造体
typedef struct {
    bool success;            // 成功・失敗のフラグ
    int32_t response_code;   // レスポンスコード
    char* message;           // レスポンスメッセージ（メモリはRust側で管理）
} Response;

#ifdef __cplusplus
}
#endif 