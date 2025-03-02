#pragma once

/* QOI RGB tag: Indicates a full RGB pixel without alpha */
#define QOI_OP_RGB_TAG 			0xfe

/* QOI RGBA tag: Indicates a full RGBA pixel */
#define QOI_OP_RGBA_TAG			0xff

/* QOI index operation: Refers to a previously seen color in the color index */
#define QOI_OP_INDEX(x) 		(uint8_t)((x) & 0x3f)

/* QOI diff operation: Encodes small color differences for RGB channels */
#define QOI_OP_DIFF(dr,dg,db) 	(uint8_t)(((((dr) & 0x03) << 4) | (((dg) & 0x03) << 2) | ((db) & 0x03)) | 0x40)

/* QOI luma operation: Encodes color difference using green as reference */
#define QOI_OP_LUMA(dr,dg,db)	{ (uint8_t)(((dg) & 0x3f) | 0x80), (uint8_t)(((((dr) - (dg)) & 0x0f) << 4) | (((db) - (dg)) & 0x0f)) }

/* QOI run-length operation: Encodes repeated pixel runs */
#define QOI_OP_RUN(x)			(uint8_t)(((x) & 0x3f) | 0xc0)
