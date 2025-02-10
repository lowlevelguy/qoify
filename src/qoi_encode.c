#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "qoi_encode.h"
#include "qoi_channels.h"
#include "qoi_header.h"
#include "qoi_operations.h"
#include "errmsg.h"

#define MAX_CHUNK_SIZE 1024

void qoi_encode(char* in_path, char* out_path, uint32_t width, uint32_t height, uint8_t has_alpha) {
	if (has_alpha)
		qoi_encode_rgba(in_path, out_path, width, height);
	else
		qoi_encode_rgb(in_path, out_path, width, height);
}

void qoi_encode_rgba(char* in_path, char* out_path, uint32_t width, uint32_t height) {
	FILE* in = fopen(in_path, "r"), *out = fopen(out_path, "w");
	if (in == NULL || out == NULL) {
		printf("Error occured when trying to open `%s` in read mode and/or `%s` in write mode.\n",
				in_path, out_path);

		if (in != NULL) fclose(in);
		if (out != NULL) fclose(out);

		return;
	}

	int errmsg = write_qoi_header(out, width, height, QOI_HEADER_CHANNELS_RGBA, QOI_HEADER_COLORSPACE_SRGB);
	if (errmsg < 0) {
		errmsg_qoi_header(errmsg);
		return;
	}

	// Supposing we're working with only RGBA here for now
	rgba_t prev_px = QOI_COLOR_RGBA_BLACK, current_px;
	rgba_t seen_px[64] = {0}, px_chunk[MAX_CHUNK_SIZE] = {0};
	uint8_t write_chunk[MAX_CHUNK_SIZE];
	size_t index, px_chunk_sz, run = 0, write_chunk_sz = 0;

	// In order to strike a balance between performance and memory usage,
	// we'll read the input file in chunks, and process them one by one.
	// This is better than reading 4 bytes per 4 bytes (RGBA), which may or
	// may not cause a performance hit, as well as loading the whole file
	// into memory at once, which may take up way too much space.
	//
	// IMPORTANT: do NOT switch up the order of size and nmemb
	while ((px_chunk_sz = fread(px_chunk, sizeof(rgba_t), MAX_CHUNK_SIZE, in)) > 0) {
		for (int i = 0; i < px_chunk_sz/sizeof(rgba_t); i++) {
			current_px = px_chunk[i];
			
			if (memcmp(&current_px, &prev_px, 4) == 0) {
				run++;
				if (run == 63) {
					write_chunk[write_chunk_sz++] = QOI_OP_RUN(run);	
					run = 0;
				}
			} else if (run > 0) {
				write_chunk[write_chunk_sz++] = QOI_OP_RUN(run);
				run = 0;
			} else {
				index = (3 * current_px.r + 5 * current_px.g + 7 * current_px.b + 11 * current_px.a) % 64;
	
				if (memcmp(&seen_px[index], &current_px, 4) == 0) {
					write_chunk[write_chunk_sz++] = QOI_OP_INDEX(index);
				} else {
					seen_px[index] = current_px;
				}
			}

			// Write to file when write_chunk is full
			if (write_chunk_sz == MAX_CHUNK_SIZE)
				fwrite(write_chunk, 1, MAX_CHUNK_SIZE, out);

			prev_px = current_px;
		}
	}

	fclose(in);
	fclose(out);
}

void qoi_encode_rgb(char* in_path, char* out_path, uint32_t width, uint32_t height) {}
