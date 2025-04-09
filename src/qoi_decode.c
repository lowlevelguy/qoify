#include <stdio.h>
#include <string.h>

#include "errmsg.h"
#include "qoi_header.h"
#include "qoi_decode.h"
#include "qoi_operations.h"
#include "qoi_channels.h"

#define MAX_BUFF_SIZE 1024

void qoi_decode(char* in_path, char* out_path, int alpha) {
	if (alpha) {
		qoi_decode_rgba(in_path, out_path);
	} else {
		qoi_decode_rgb(in_path, out_path);
	}
}

void qoi_decode_rgba(char* in_path, char* out_path) {
	FILE* in = fopen(in_path, "r"), *out = fopen(out_path, "w");
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

	uint8_t read_buff[MAX_BUFF_SIZE];
	rgba_t write_buff[MAX_BUFF_SIZE], prev_px = QOI_COLOR_RGBA_BLACK;
	size_t read_buff_sz = 0, write_buff_sz = 0;
	while ((read_buff_sz = fread(read_buff, 1, MAX_BUFF_SIZE, in)) > 0) {
		for (int i = 0; i < read_buff_sz; i++) {
			// Begin by checking for RGBA/RGB operations
			if (read_buff[i] == QOI_OP_RGBA_TAG) {
				// If we detect an RGBA operation that has been split over two buffered
				// reads, we simply retreat the cursor to right before the operation and
				// skip to the next buffered read
				if (i >= read_buff_sz-sizeof(rgba_t)) {
					fseek(out, -sizeof(rgba_t), SEEK_CUR);
					break;
				}

				memcpy(&write_buff[write_buff_sz++], &read_buff[i+1], sizeof(rgba_t));
				i += sizeof(rgba_t);
			}
			
			else if (read_buff[i] == QOI_OP_RGB_TAG) {
				// Same bound checking as for RGBA
				if (i >= read_buff_sz-3) {
					fseek(out, -3, SEEK_CUR);
					break;
				}
				
				memcpy(&write_buff[write_buff_sz++], &read_buff[i+1], sizeof(rgb_t));
				write_buff[write_buff_sz].a = prev_px.a;
				i += sizeof(rgb_t);
			}

			


			prev_px = write_buff[write_buff_sz];

			if (write_buff_sz == MAX_BUFF_SIZE) {
				fwrite(write_buff, sizeof(rgba_t), MAX_BUFF_SIZE, out);
				write_buff_sz = 0;
			}
		}
	}

	fclose(in);
	fclose(out);
}

void qoi_decode_rgb(char* in_path, char* out_path) {

}
