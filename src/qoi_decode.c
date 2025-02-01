#include <stdio.h>

#include "errmsg.h"
#include "qoi_header.h"
#include "qoi_decode.h"

// returns 1 on success
// 0 or negative number on fail
void qoi_decode(char* in_path, char* out_path) {
	FILE* in = fopen(in_path, "r"), *out = fopen(out_path, "w");
	if (in == NULL || out == NULL) {
		printf("Error occured when trying to open `%s` in read mode and/or `%s` in write mode.\n",
				in_path, out_path);
		return;
	}

	qoi_header qh;
	int errcode;
	if ((errcode = read_qoi_header(in, &qh)) <= 0) {
		printf(errs_qoi_header[-errcode].msg, in_path);
		return;
	}

}
