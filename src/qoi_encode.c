#include <stdint.h>
#include <stdio.h>

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
	// Opening in binary mode for extended portability
	FILE* in = fopen(in_path, "rb"), *out = fopen(out_path, "wb");
	if (in == NULL || out == NULL) {
		fprintf(stderr, "Error occured when trying to open `%s` in read mode and/or `%s` in write mode.\n",
				in_path, out_path);

		if (in != NULL) fclose(in);
		if (out != NULL) fclose(out);

		return;
	}

	int errmsg = write_qoi_header(out, width, height, QOI_HEADER_CHANNELS_RGBA, QOI_HEADER_COLORSPACE_SRGB);
	if (errmsg < 0) {
		fprintf(stderr, "%s", errmsg_qoi_header(errmsg));
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
		for (int i = 0; i < px_chunk_sz; i++) {
			current_px = px_chunk[i];

			// Try: run-length encoding
			if (current_px.r == prev_px.r &&
				current_px.g == prev_px.g &&
				current_px.b == prev_px.b &&
				current_px.a == prev_px.a) {

				run++;
				// The run length is encoded over 6 bits with a bias of -1
				// The values 63 and 64 are illegal, since they would be
				// encoded as 0b111110 and 0b111111 respectively, which would
				// conflict with the 8-bit tag for RGB and RGBA sequences.
				if (run == 62) {
					write_chunk[write_chunk_sz++] = QOI_OP_RUN(run-1);
					run = 0;
				}
			}

			// If the current pixel differs from the previous
			else {
				if (run > 0) {
					write_chunk[write_chunk_sz++] = QOI_OP_RUN(run-1);
					run = 0;

					if (write_chunk_sz == MAX_CHUNK_SIZE) {
						fwrite(write_chunk, 1, write_chunk_sz, out);
						write_chunk_sz = 0;
					}
				}
				
				index = (3 * current_px.r + 5 * current_px.g + 7 * current_px.b + 11 * current_px.a) % 64;
	
				// Try: previously-seen pixel index encoding
				if (seen_px[index].r == current_px.r &&
					seen_px[index].g == current_px.g &&
					seen_px[index].b == current_px.b &&
					seen_px[index].a == current_px.a) {

					write_chunk[write_chunk_sz++] = QOI_OP_INDEX(index);

				} else {
					seen_px[index] = current_px;
					
					int8_t dr = current_px.r - prev_px.r,
						   dg = current_px.g - prev_px.g,
						   db = current_px.b - prev_px.b;
		
					if (current_px.a == prev_px.a) {
						if (dr >= -2 && dr <= 1 &&
							dg >= -2 && dg <= 1 &&
							db >= -2 && db <= 1) {
							
							write_chunk[write_chunk_sz++] = QOI_OP_DIFF(dr, dg, db);
						}
		
						// Try: LUMA encoding
						else if (dg >= -32 && dg <= 31 &&
								 dr-dg >= -8 && dr-dg <= 7 &&
								 db-dg >= -8 && db-dg <= 7) {
		
							// Write to file if there are no 2 free bytes
							if (write_chunk_sz >= MAX_CHUNK_SIZE-2) {
								fwrite(write_chunk, 1, write_chunk_sz, out);
								write_chunk_sz = 0;
							}
		
							uint8_t bytes[] = QOI_OP_LUMA(dr, dg, db);
							write_chunk[write_chunk_sz++] = bytes[0];
							write_chunk[write_chunk_sz++] = bytes[1];
						}
		
						// If all fails, but the alpha still matches, use RGB encoding
						else {
							// Write to file if there are no 4 free bytes
							if (write_chunk_sz >= MAX_CHUNK_SIZE-4) {
								fwrite(write_chunk, 1, write_chunk_sz, out);
								write_chunk_sz = 0;
							}
		
							write_chunk[write_chunk_sz++] = QOI_OP_RGB_TAG;
							write_chunk[write_chunk_sz++] = current_px.r;
							write_chunk[write_chunk_sz++] = current_px.g;
							write_chunk[write_chunk_sz++] = current_px.b;
						}
					}
					
					// If the alpha values don't match, jump to writing out the RGBA pixel in full
					else {
						// Write to file if there are no 5 free bytes
						if (write_chunk_sz >= MAX_CHUNK_SIZE-5) {
							fwrite(write_chunk, 1, write_chunk_sz, out);
							write_chunk_sz = 0;
						}
		
						write_chunk[write_chunk_sz++] = QOI_OP_RGBA_TAG;
						write_chunk[write_chunk_sz++] = current_px.r;
						write_chunk[write_chunk_sz++] = current_px.g;
						write_chunk[write_chunk_sz++] = current_px.b;
						write_chunk[write_chunk_sz++] = current_px.a;
					}
				}
			}
		

			// Write to file when write_chunk is full
			if (write_chunk_sz == MAX_CHUNK_SIZE) {
				fwrite(write_chunk, 1, write_chunk_sz, out);
				write_chunk_sz = 0;
			}

			prev_px = current_px;
		}
	}

	// Write remaining run-length data if it exists
	if (run > 0) {
		uint8_t byte = QOI_OP_RUN(run-1);
		fwrite(&byte, 1, 1, out);
	}

	// Write remaining data in write_chunk to file if it exists
	if (write_chunk_sz > 0)
		fwrite(write_chunk, 1, write_chunk_sz, out);

	// Write QOI stream ending marker bytes
	uint8_t end[] = {0,0,0,0,0,0,0,1};
	fwrite(end, 1, 8, out);

	fclose(in);
	fclose(out);
}

void qoi_encode_rgb(char* in_path, char* out_path, uint32_t width, uint32_t height) {
	// Opening in binary mode for extended portability
	FILE* in = fopen(in_path, "rb"), *out = fopen(out_path, "wb");
	if (in == NULL || out == NULL) {
		fprintf(stderr, "Error occured when trying to open `%s` in read mode and/or `%s` in write mode.\n",
				in_path, out_path);

		if (in != NULL) fclose(in);
		if (out != NULL) fclose(out);

		return;
	}

	int errmsg = write_qoi_header(out, width, height, QOI_HEADER_CHANNELS_RGB, QOI_HEADER_COLORSPACE_SRGB);
	if (errmsg < 0) {
		fprintf(stderr, "%s", errmsg_qoi_header(errmsg));
		return;
	}

	// Supposing we're working with only RGBA here for now
	rgb_t prev_px = QOI_COLOR_RGB_BLACK, current_px;
	rgb_t seen_px[64] = {0}, px_chunk[MAX_CHUNK_SIZE] = {0};
	uint8_t write_chunk[MAX_CHUNK_SIZE];
	size_t index, px_chunk_sz, run = 0, write_chunk_sz = 0;

	// In order to strike a balance between performance and memory usage,
	// we'll read the input file in chunks, and process them one by one.
	// This is better than reading 4 bytes per 4 bytes (RGBA), which may or
	// may not cause a performance hit, as well as loading the whole file
	// into memory at once, which may take up way too much space.
	//
	// IMPORTANT: do NOT switch up the order of size and nmemb
	while ((px_chunk_sz = fread(px_chunk, sizeof(rgb_t), MAX_CHUNK_SIZE, in)) > 0) {
		for (int i = 0; i < px_chunk_sz; i++) {
			current_px = px_chunk[i];

			// Try: run-length encoding
			if (current_px.r == prev_px.r &&
				current_px.g == prev_px.g &&
				current_px.b == prev_px.b) {

				run++;
				// The run length is encoded over 6 bits with a bias of -1
				// The values 63 and 64 are illegal, since they would be
				// encoded as 0b111110 and 0b111111 respectively, which would
				// conflict with the 8-bit tag for RGB and RGBA sequences.
				if (run == 62) {
					write_chunk[write_chunk_sz++] = QOI_OP_RUN(run-1);
					run = 0;
				}
			}

			// If the current pixel differs from the previous
			else {
				if (run > 0) {
					write_chunk[write_chunk_sz++] = QOI_OP_RUN(run-1);
					run = 0;

					if (write_chunk_sz == MAX_CHUNK_SIZE) {
						fwrite(write_chunk, 1, write_chunk_sz, out);
						write_chunk_sz = 0;
					}
				}
				
				// Alpha channel is considered to be 255
				index = (3 * current_px.r + 5 * current_px.g + 7 * current_px.b + 11 * 255) % 64;
	
				// Try: previously-seen pixel index encoding
				if (seen_px[index].r == current_px.r &&
					seen_px[index].g == current_px.g &&
					seen_px[index].b == current_px.b) {

					write_chunk[write_chunk_sz++] = QOI_OP_INDEX(index);

				} else {
					seen_px[index] = current_px;
					
					int8_t dr = current_px.r - prev_px.r,
						   dg = current_px.g - prev_px.g,
						   db = current_px.b - prev_px.b;
		
					if (dr >= -2 && dr <= 1 &&
						dg >= -2 && dg <= 1 &&
						db >= -2 && db <= 1) {
						
						write_chunk[write_chunk_sz++] = QOI_OP_DIFF(dr, dg, db);
					}
		
					// Try: LUMA encoding
					else if (dg >= -32 && dg <= 31 &&
							 dr-dg >= -8 && dr-dg <= 7 &&
							 db-dg >= -8 && db-dg <= 7) {
		
						// Write to file if there are no 2 free bytes
						if (write_chunk_sz >= MAX_CHUNK_SIZE-2) {
							fwrite(write_chunk, 1, write_chunk_sz, out);
							write_chunk_sz = 0;
						}
		
						uint8_t bytes[] = QOI_OP_LUMA(dr, dg, db);
						write_chunk[write_chunk_sz++] = bytes[0];
						write_chunk[write_chunk_sz++] = bytes[1];
					}
		
					// If all fails, but the alpha still matches, use RGB encoding
					else {
						// Write to file if there are no 4 free bytes
						if (write_chunk_sz >= MAX_CHUNK_SIZE-4) {
							fwrite(write_chunk, 1, write_chunk_sz, out);
							write_chunk_sz = 0;
						}
		
						write_chunk[write_chunk_sz++] = QOI_OP_RGB_TAG;
						write_chunk[write_chunk_sz++] = current_px.r;
						write_chunk[write_chunk_sz++] = current_px.g;
						write_chunk[write_chunk_sz++] = current_px.b;
					}
				}
			}
		

			// Write to file when write_chunk is full
			if (write_chunk_sz == MAX_CHUNK_SIZE) {
				fwrite(write_chunk, 1, write_chunk_sz, out);
				write_chunk_sz = 0;
			}

			prev_px = current_px;
		}
	}

	// Write remaining run-length data if it exists
	if (run > 0) {
		uint8_t byte = QOI_OP_RUN(run-1);
		fwrite(&byte, 1, 1, out);
	}

	// Write remaining data in write_chunk to file if it exists
	if (write_chunk_sz > 0)
		fwrite(write_chunk, 1, write_chunk_sz, out);

	// Write QOI stream ending marker bytes
	uint8_t end[] = {0,0,0,0,0,0,0,1};
	fwrite(end, 1, 8, out);

	fclose(in);
	fclose(out);
}
