#include "errmsg.h"
#include "qoi_header.h"

errmsg errs_qoi_header[] = {
	{FILE_READ_ERROR, "Error occured when trying to read from `%s`.\n"},
	{FILE_WRITE_ERROR, "Error occured when trying to write to `%s`.\n"},
	{QOI_HEADER_CHANNELS_INVALID, "Invalid color channels used in `%s`.\n"},
	{QOI_HEADER_COLORSPACE_INVALID, "Invalid colorspace used in `%s`.\n"}
}; 
