#pragma once

#include <stdio.h>

void qoi_decode(char* in_path, char* out_path, int alpha);
void qoi_decode_rgba(char* in_path, char* out_path);
void qoi_decode_rgb(char* in_path, char* out_path);
