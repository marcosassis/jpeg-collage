#ifndef LIBJPEG_WRAPPER_MARKERS_H
#define LIBJPEG_WRAPPER_MARKERS_H

#include "libjpeg_wrapper_include.h"

/* optional: this must be called before jpeg_read_header() */
void libjpeg_copy_markers_setup(j_decompress_ptr srcinfo, boolean copy_all);

/* optional: this must be called just after jpeg_write_coefficients() */
void libjpeg_copy_markers_execute(j_decompress_ptr srcinfo, j_compress_ptr dstinfo);

#endif
