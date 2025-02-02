#include <stdio.h>

#include "errmsg.h"
#include "qoi_header.h"
#include "qoi_decode.h"

void qoi_decode(char* in_path, char* out_path) {
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

}
