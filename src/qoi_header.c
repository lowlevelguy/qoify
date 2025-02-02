#include "qoi_header.h"

/* Returns 0 on success, negative number on failure */
int read_qoi_header(FILE* f, qoi_header* qh) {
	// set file position to file's beginning
	fseek(f, 0, SEEK_SET);

	// get magic bytes
	if (fread(qh->magic, 4, 1, f) == 0) {
		return FILE_READ_ERROR;
	}

	// get width and height
	if (fread(&qh->width, 4, 1, f) == 0 || fread(&qh->height, 4, 1, f) == 0) {
		return FILE_READ_ERROR;
	}

	// get channels
	uint8_t ch;
	if (fread(&ch, 1, 1, f) == 0) {
		return FILE_READ_ERROR;
	}
	if (ch != QOI_HEADER_CHANNELS_RGB && ch != QOI_HEADER_CHANNELS_RGBA) {
		return QOI_HEADER_CHANNELS_INVALID;
	}
	qh->channels = ch;

	// get colorspace
	uint8_t cs;
	if (fread(&cs, 1, 1, f) == 0) {
		return FILE_READ_ERROR;
	}
	if (cs != QOI_HEADER_COLORSPACE_SRGB && cs != QOI_HEADER_COLORSPACE_LINEAR) {
		return QOI_HEADER_COLORSPACE_INVALID;
	}
	qh->colorspace = cs;

	return 0;
}

/* Returns 0 on success, negative number on failure */
int write_qoi_header(FILE* f, uint32_t width, uint32_t height, uint8_t channels, uint8_t colorspace) {
	// set file position to file's beginning
	fseek(f, 0, SEEK_SET);

	// write magic bytes
	if (fwrite("qoif", 4, 1, f) == 0) {
		return FILE_WRITE_ERROR;
	}
	
	// write width and height 
	if (fwrite(&width, 4, 1, f) == 0 || fwrite(&height, 4, 1, f) == 0) {
		return FILE_WRITE_ERROR;
	}

	// check then write channels
	if (channels != QOI_HEADER_CHANNELS_RGB && channels != QOI_HEADER_CHANNELS_RGBA) {
		return 	QOI_HEADER_CHANNELS_INVALID;
	}
	if (fwrite(&channels, 1, 1, f) == 0) {
		return FILE_WRITE_ERROR;
	}

	// check then write colorspace
	if (colorspace != QOI_HEADER_COLORSPACE_SRGB && colorspace != QOI_HEADER_COLORSPACE_LINEAR) {
		return QOI_HEADER_COLORSPACE_INVALID;
	}
	if (fwrite(&colorspace, 1, 1, f) == 0) {
		return FILE_WRITE_ERROR;
	}

	return 0;
}
