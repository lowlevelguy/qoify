#pragma once

typedef struct {
	int errcode;
	const char* msg;
} errmsg;

const char* errmsg_qoi_header(int errcode);
