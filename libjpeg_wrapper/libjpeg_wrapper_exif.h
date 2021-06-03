#ifndef LIBJPEG_WRAPPER_EXIF_H
#define LIBJPEG_WRAPPER_EXIF_H

#include <jinclude.h>
#include <jpeglib.h>

void libjpeg_adjust_exif_dimensions(jpeg_saved_marker_ptr markers, j_compress_ptr dst_info);

#endif
