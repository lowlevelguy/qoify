#pragma once

#include <stdint.h>

/* QOI RGB tag: Indicates a full RGB pixel without alpha */
#define QOI_OP_RGB_TAG 			0xfe

/* QOI RGBA tag: Indicates a full RGBA pixel */
#define QOI_OP_RGBA_TAG			0xff

/* QOI index operation: Refers to a previously seen color in the color index */
#define QOI_OP_INDEX_TAG		0x00

/* QOI diff operation: Encodes small color differences for RGB channels */
#define QOI_OP_DIFF_TAG			0x40

/* QOI luma operation: Encodes color difference using green as reference */
#define QOI_OP_LUMA_TAG			0x80

/* QOI run-length operation: Encodes repeated pixel runs */
#define QOI_OP_RUN_TAG			0xc0


uint8_t qoi_op_index (uint8_t i);
uint8_t qoi_op_diff (int8_t dr, int8_t dg, int8_t db);
void qoi_op_luma (int8_t dr, int8_t dg, int8_t db, uint8_t bytes[2]);
uint8_t qoi_op_run (uint8_t run);
