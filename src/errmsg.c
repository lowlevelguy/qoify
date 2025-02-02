#include "errmsg.h"
#include "qoi_header.h"

const char* errmsg_qoi_header(int errcode) {
	const errmsg errs_qoi_header[] = {
		{FILE_READ_ERROR, "Error occurred when trying to read from `%s`.\n"},
		{FILE_WRITE_ERROR, "Error occurred when trying to write to `%s`.\n"},
		{QOI_HEADER_CHANNELS_INVALID, "Invalid color channels used in `%s`.\n"},
		{QOI_HEADER_COLORSPACE_INVALID, "Invalid colorspace used in `%s`.\n"}
	};

	for (size_t i = 0; i < sizeof(errs_qoi_header)/sizeof(errmsg); i++) {
		if (errcode == errs_qoi_header[i].errcode)
			return errs_qoi_header[i].msg;
	}
	return "Unknown error.\n";
}
