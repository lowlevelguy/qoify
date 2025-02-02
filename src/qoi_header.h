#pragma once

#include <stdio.h>
#include <stdint.h>

#define FILE_READ_ERROR 				-1
#define FILE_WRITE_ERROR				-2
#define QOI_HEADER_CHANNELS_INVALID 	-3
#define QOI_HEADER_COLORSPACE_INVALID 	-4

enum {
	QOI_HEADER_CHANNELS_RGB = 3,
	QOI_HEADER_CHANNELS_RGBA
};

enum {
	QOI_HEADER_COLORSPACE_SRGB = 0,
	QOI_HEADER_COLORSPACE_LINEAR
};

typedef struct {
	char magic[4];					// magic bytes "qoif"
	uint32_t width, height;			// in pixels
	uint8_t channels,				// 3 = RGB, 4 = RGBA
			colorspace;				// 0 = sRGB with linear alpha, 1 = all channels linear
} qoi_header;

int read_qoi_header(FILE* f, qoi_header* qh);
int write_qoi_header(FILE* f, uint32_t widht, uint32_t height, uint8_t channels, uint8_t colorspace);
