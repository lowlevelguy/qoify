#pragma once

#include <stdio.h>

void qoi_decode(char* in_path, char* out_path);
void qoi_decode_rgba(FILE* in_path, FILE* out);
void qoi_decode_rgb(FILE* in_path, FILE* out_path);
