#include <stdio.h>
#include <string.h>

#include "qoi_decode.h"
#include "qoi_encode.h"

int main (int argc, char** argv) {
	if (argc < 4) {
		printf(
		    "qoify - Convert between RAW and QOI image formats\n\n"
		    "Usage: qoify [mode] input output\n\n"
		    "Modes:\n"
		    "  -d    Decode QOI file to RAW format\n"
		    "  -e    Encode RAW file to QOI format\n\n"
		    "Examples:\n"
		    "  qoify -e image.raw image.qoi    # Encode RAW to QOI\n"
		    "  qoify -d image.qoi image.raw    # Decode QOI to RAW\n\n"
		    "Note: RAW files must be uncompressed RGB/RGBA data\n"
		);
		return -1;
	}
	if (strncmp("-d", argv[1], 2) == 0) {
		qoi_decode(argv[2], argv[3]);
	} else if (strncmp("-e", argv[1], 2) == 0) {
		qoi_encode(argv[2], argv[3]);
	} else {
		printf("Error: Unknown mode `%s`.\n", argv[1]);
		return -1;
	}
	
	return 0;
}
