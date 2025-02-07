#pragma once
#include <stdint.h>

typedef struct {
	uint8_t r, g, b;
} rgb_t;

typedef struct {
	uint8_t r, g, b, a;
} rgba_t;

rgb_t RGB(uint8_t r, uint8_t g, uint8_t b);
rgba_t RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/* Commonly used colors */
// RGB
#define QOI_COLOR_RGB_BLACK 	RGB(0, 0, 0)
#define QOI_COLOR_RGB_WHITE 	RGB(255, 255, 255)
#define QOI_COLOR_RGB_RED 		RGB(255, 0, 0)
#define QOI_COLOR_RGB_GREEN 	RGB(0, 255, 0)
#define QOI_COLOR_RGB_BLUE 		RGB(0, 0, 255)

// RGBA
#define QOI_COLOR_RGBA_BLACK 	RGBA(0, 0, 0, 255)
#define QOI_COLOR_RGBA_WHITE 	RGBA(255, 255, 255, 255)
#define QOI_COLOR_RGBA_RED 		RGBA(255, 0, 0, 255)
#define QOI_COLOR_RGBA_GREEN 	RGBA(0, 255, 0, 255)
#define QOI_COLOR_RGBA_BLUE 	RGBA(0, 0, 255, 255)
