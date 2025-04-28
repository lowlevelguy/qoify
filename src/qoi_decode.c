#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ucontext.h>

#include "errmsg.h"
#include "qoi_header.h"
#include "qoi_decode.h"
#include "qoi_operations.h"
#include "qoi_channels.h"

#define MAX_CHUNK_SIZE 1024

void qoi_decode (char* in_path, char* out_path) {
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

	if (qh.channels == 4) {
		qoi_decode_rgba(in, out);
	} else {
		qoi_decode_rgb(in, out);
	}
}

void qoi_decode_rgba (FILE* in, FILE* out) {
	fseek(in, sizeof(qoi_header), SEEK_SET);
	if (ferror(in)) {
		perror("qoi_decode::qoi_decode_rgba: Could not seek within file: ");
		fclose(in);
		fclose(out);
		return;
	}

	uint8_t read_chunk[MAX_CHUNK_SIZE], dr, dg, db, dr_dg, db_dg;
	rgba_t seen_px[64], write_chunk[MAX_CHUNK_SIZE], prev_px = QOI_COLOR_RGBA_BLACK;
	size_t read_chunk_sz = 0, write_chunk_sz = 0, run_length = 0, index;

	while ((read_chunk_sz = fread(read_chunk, 1, MAX_CHUNK_SIZE, in)) > 0) {
		for (int i = 0; i < read_chunk_sz; i++) {
			// To detect the end marker bytes, we need to detect two consecutive zero bytes.
			// To avoid edge cases, whenever we detect a zero byte, we retreat the cursor.
			//
			// For a given index i, the number of elements left until the end of the
			// read_chunk array (element of index i included) is: read_chunk_sz - i.
			if (read_chunk[i] == 0) {
				if (i >= read_chunk_sz-1) {
					fseek(in, -(read_chunk_sz - i), SEEK_CUR);
					break;
				}
	
				// If we detect two consecutive zeroes, notice that only two possibilities
				// exist that are specification-compliant: either it's the end marker bytes,
				// or they are within an RGBA/RGB operation.
				// The latter are processed simultaneously when an RGBA/RGB tag is detected,
				// so the only remaining possibility is the latter.
				if (read_chunk[i+1] == 0)
					goto end_marker_check;
			}

			// Begin by checking for RGBA/RGB operations
			if (read_chunk[i] == QOI_OP_RGBA_TAG) {
				// If we detect an RGBA operation that has been split over two chunkered
				// reads, we simply retreat the cursor to right before the operation and
				// skip to the next buffered read.
				if (i >= read_chunk_sz - sizeof(rgba_t)) {
					fseek(in, -(read_chunk_sz - i), SEEK_CUR);
					break;
				}

				memcpy(&write_chunk[write_chunk_sz++], &read_chunk[i+1], sizeof(rgba_t));
				i += sizeof(rgba_t);
			}
			
			else if (read_chunk[i] == QOI_OP_RGB_TAG) {
				// Same bound checking as for RGBA
				if (i >= read_chunk_sz - sizeof(rgb_t)) {
					fseek(in, -(read_chunk_sz - i), SEEK_CUR);
					break;
				}
				
				memcpy(&write_chunk[write_chunk_sz], &read_chunk[i+1], sizeof(rgb_t));
				write_chunk[write_chunk_sz++].a = prev_px.a;
				i += sizeof(rgb_t);
			}

			else if ((read_chunk[i] & 0xc0) == QOI_OP_RUN_TAG) {
				run_length = (read_chunk[i] & 0x3f) + 1;
				
				// Write to file if the buffer is too saturated
				if (write_chunk_sz >= MAX_CHUNK_SIZE - run_length) {
					fwrite(write_chunk, sizeof(rgba_t), write_chunk_sz, out);
					if (ferror(out)) {
						perror("qoi_decode::qoi_decode_rgba: Could not write to file: ");
						fclose(in);
						fclose(out);
						return;
					}

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

			else { 
				if ((read_chunk[i] & 0xc0) == QOI_OP_DIFF_TAG) {
					// Bit masks:
					// 	- 0x30 = 0b00110000
					// 	- 0x0c = 0b00001100
					// 	- 0x03 = 0b00000011
					// The diffs were saved with a bias of +2, so remember to remove it.
					dr = ((read_chunk[i] & 0x30) >> 4) - 2;
					dg = ((read_chunk[i] & 0x0c) >> 2) - 2;
					db = (read_chunk[i] & 0x03) - 2;
				}
				
				else if ((read_chunk[i] & 0xc0) == QOI_OP_LUMA_TAG) {
					// A LUMA operation takes up two bytes, so use a similar bound check
					// to the one used for RGBA/RGB.
					if (i >= read_chunk_sz - 1) {
						fseek(in, -1, SEEK_CUR);
						break;
					}

					// Bit masks:
					// 	- 0x3f = 0b00111111
					// 	- 0xf0 = 0b11110000
					// 	- 0x0f = 0b00001111
					//
					// The differences dr and db are with a bias of +8, while dg of +32.
					dg = (read_chunk[i++] & 0x3f) - 32;
					dr_dg = ((read_chunk[i] & 0xf0) >> 4) - 8;
					db_dg = (read_chunk[i] & 0x0f) - 8;

					dr = dr_dg + dg;
					db = db_dg + dg;
				}
				
				write_chunk[write_chunk_sz].r = prev_px.r + dr;
				write_chunk[write_chunk_sz].g = prev_px.g + dg;
				write_chunk[write_chunk_sz].b = prev_px.b + db;
				write_chunk[write_chunk_sz++].a = prev_px.a;
			}

			prev_px = write_chunk[write_chunk_sz-1];
			index = (3 * prev_px.r + 5 * prev_px.g + 7 * prev_px.b + 11 * prev_px.a) % 64;
			seen_px[index] = prev_px;

			if (write_chunk_sz == MAX_CHUNK_SIZE) {
				fwrite(write_chunk, sizeof(rgba_t), MAX_CHUNK_SIZE, out);
				if (ferror(out)) {
					perror("qoi_decode::qoi_decode_rgba: Could not write to file: ");
					fclose(in);
					fclose(out);
					return;
				}

				write_chunk_sz = 0;
			}
		}
	}

	if (ferror(in)) {
		perror("qoi_decode::qoi_decode_rgba: Could not read from file: ");
		fclose(in);
		fclose(out);
		return;
	}

end_marker_check:
	// Seek to read exactly the last 8 bytes
	fseek(in, -8, SEEK_END);
	if (ferror(in)) {
		perror("qoi_decode::qoi_decode_rgba: Could not seek within file: ");
		fclose(in);
		fclose(out);
		return;
	}

	uint8_t end_marker[8] = {0}, bytes[8];
	end_marker[7] = 1;

	fread(bytes, 1, 8, in);

	if (memcmp(end_marker, bytes, 8) != 0) {
		fprintf(stderr, "Warning: Missing 8 byte marker. File may be corrupted.");
	}
	

	if (write_chunk_sz > 0) {
		fwrite(write_chunk, sizeof(rgba_t), write_chunk_sz, out);
		if (ferror(out))
			perror("qoi_decode::qoi_decode_rgba: Could not write to file.");
	}

	fclose(in);
	fclose(out);
}

void qoi_decode_rgb (FILE* in, FILE* out) {

}
