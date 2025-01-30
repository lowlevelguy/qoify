#include <stdio.h>
#include <string.h>

#include "qoi_decode.h"
#include "qoi_encode.h"

int main (int argc, char** argv) {
	if (argc < 4) {
		printf("Usage: qoify [mode] input output\n\n"
			   "Modes:\n"
			   "\t-d\tdecode\n"
			   "\t-e\tencode\n");
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
