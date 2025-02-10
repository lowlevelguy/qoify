#pragma once

#include <stdint.h>

void qoi_encode(char* in_path, char* out_path, uint32_t width, uint32_t height, uint8_t has_alpha);
void qoi_encode_rgba(char* in_path, char* out_path, uint32_t width, uint32_t height);
void qoi_encode_rgb(char* in_path, char* out_path, uint32_t width, uint32_t height);
