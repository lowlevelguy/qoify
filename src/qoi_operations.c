#include "qoi_operations.h"

uint8_t qoi_op_index (uint8_t i) {
	return QOI_OP_INDEX_TAG | (i & 0x3f);
}

uint8_t qoi_op_diff (int8_t dr, int8_t dg, int8_t db) {
	// all diffs are encoded with a bias of +2
	return (QOI_OP_DIFF_TAG | 
		(((dr+2) & 0x03) << 4) | 
		(((dg+2) & 0x03) << 2) | 
		((db+2) & 0x03)
	);
}

void qoi_op_luma (int8_t dr, int8_t dg, int8_t db, uint8_t bytes[2]) {
	uint8_t dr_dg = dr - dg, db_dg = db - dg;

	// dg is encoded with a bias of +32,
	// and dr_dg, db_dg with a bias of +8
	bytes[0] = QOI_OP_LUMA_TAG | ((dg+32) & 0x3f);
	bytes[1] = (((dr_dg+8) & 0x0f) << 4) | ((db_dg+8) & 0x0f);
}

// Run length is encoded with a bias of -1
uint8_t qoi_op_run (uint8_t run) {
	return QOI_OP_RUN_TAG | ((run-1) & 0x3f);
}
