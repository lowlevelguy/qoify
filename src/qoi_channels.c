#include "qoi_channels.h"

rgb_t RGB(uint8_t r, uint8_t g, uint8_t b) {
	return (rgb_t) {.r = r, .g = g, .b = b};
}

rgba_t RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return (rgba_t) {.r = r, .g = g, .b = b, .a = a};
}
