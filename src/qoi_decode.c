#include <stdio.h>
#include <string.h>

#include "errmsg.h"
#include "qoi_header.h"
#include "qoi_decode.h"
#include "qoi_operations.h"
#include "qoi_channels.h"

#define MAX_CHUNK_SIZE 1024

void qoi_decode(char* in_path, char* out_path, int alpha) {
	if (alpha) {
		qoi_decode_rgba(in_path, out_path);
	} else {
		qoi_decode_rgb(in_path, out_path);
	}
}

void qoi_decode_rgba(char* in_path, char* out_path) {
	FILE* in = fopen(in_path, "rb"), *out = fopen(out_path, "wb");
	if (in == NULL || out == NULL) {
		printf("Error occured when trying to open `%s` in read mode and/or `%s` in write mode.\n",
				in_path, out_path);
		return;
	}

	qoi_header qh;
	int errcode;
	if ((errcode = read_qoi_header(in, &qh)) < 0) {
		printf(errmsg_qoi_header(errcode), in_path);
		return;
	}

	uint8_t read_chunk[MAX_CHUNK_SIZE];
	rgba_t seen_px[64], write_chunk[MAX_CHUNK_SIZE], prev_px = QOI_COLOR_RGBA_BLACK;
	size_t read_chunk_sz = 0, write_chunk_sz = 0, run_length = 0, index;

	while ((read_chunk_sz = fread(read_chunk, 1, MAX_CHUNK_SIZE, in)) > 0) {
		for (int i = 0; i < read_chunk_sz; i++) {
			// Begin by checking for RGBA/RGB operations
			if (read_chunk[i] == QOI_OP_RGBA_TAG) {
				// If we detect an RGBA operation that has been split over two chunkered
				// reads, we simply retreat the cursor to right before the operation and
				// skip to the next chunkered read
				if (i >= read_chunk_sz-sizeof(rgba_t)) {
					fseek(in, -sizeof(rgba_t), SEEK_CUR);
					break;
				}

				memcpy(&write_chunk[write_chunk_sz++], &read_chunk[i+1], sizeof(rgba_t));
				i += sizeof(rgba_t);
			}
			
			else if (read_chunk[i] == QOI_OP_RGB_TAG) {
				// Same bound checking as for RGBA
				if (i >= read_chunk_sz-3) {
					fseek(out, -3, SEEK_CUR);
					break;
				}
				
				memcpy(&write_chunk[write_chunk_sz++], &read_chunk[i+1], sizeof(rgb_t));
				write_chunk[write_chunk_sz].a = prev_px.a;
				i += sizeof(rgb_t);
			}

			else if ((read_chunk[i] & 0xc0) == QOI_OP_RUN_TAG) {
				run_length = (read_chunk[i] & 0x3f) + 1;
				
				// Write to file if the buffer is too saturated
				if (write_chunk_sz >= MAX_CHUNK_SIZE-run_length) {
					fwrite(write_chunk, sizeof(rgba_t), write_chunk_sz, out);
					write_chunk_sz = 0;
				}

				for (int j = 0; j < run_length; j++) {
					memcpy(&write_chunk[write_chunk_sz], &prev_px, sizeof(rgba_t));
					write_chunk_sz++;
				}
			}
			
			else if ((read_chunk[i] & 0xc0) == QOI_OP_INDEX_TAG) {
				index = read_chunk[i] & 0x3f;
				write_chunk[write_chunk_sz++] = seen_px[index];
			}

			prev_px = write_chunk[write_chunk_sz-1];
			index = (3 * prev_px.r + 5 * prev_px.g + 7 * prev_px.b + 11 * prev_px.a) % 64;
			seen_px[index] = prev_px;

			if (write_chunk_sz == MAX_CHUNK_SIZE) {
				fwrite(write_chunk, sizeof(rgba_t), MAX_CHUNK_SIZE, out);
				write_chunk_sz = 0;
			}
		}
	}

	fclose(in);
	fclose(out);
}

void qoi_decode_rgb(char* in_path, char* out_path) {

}
