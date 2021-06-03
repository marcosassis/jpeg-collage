/* based on libjpeg original code */

#include "libjpeg_wrapper_markers.h"

#define IS_JFIF_MARKER(marker) (marker->marker == JPEG_APP0 &&        \
                                GETJOCTET(marker->data[0]) == 0x4A && \
                                GETJOCTET(marker->data[1]) == 0x46 && \
                                GETJOCTET(marker->data[2]) == 0x49 && \
                                GETJOCTET(marker->data[3]) == 0x46 && \
                                GETJOCTET(marker->data[4]) == 0)

#define IS_ADOBE_MARKER(marker) (marker->marker == JPEG_APP0 + 14 &&   \
                                 GETJOCTET(marker->data[0]) == 0x41 && \
                                 GETJOCTET(marker->data[1]) == 0x64 && \
                                 GETJOCTET(marker->data[2]) == 0x6F && \
                                 GETJOCTET(marker->data[3]) == 0x62 && \
                                 GETJOCTET(marker->data[4]) == 0x65)


/* this must be called before jpeg_read_header() */
void libjpeg_copy_markers_setup(j_decompress_ptr srcinfo, boolean copy_all)
{
    int m;
    jpeg_save_markers(srcinfo, JPEG_COM, 0xFFFF);
    if (copy_all)
        for (m = 0; m < 16; m++)
            jpeg_save_markers(srcinfo, JPEG_APP0 + m, 0xFFFF);
}

/* this must be called just after jpeg_write_coefficients() */
void libjpeg_copy_markers_execute(j_decompress_ptr srcinfo, j_compress_ptr dstinfo)
{
    jpeg_saved_marker_ptr marker;

    for (marker = srcinfo->marker_list; marker != NULL; marker = marker->next)
    {
        /* reject duplicate JFIF or ADOBE markers */
        if (marker->data_length >= 5 &&
            ((dstinfo->write_JFIF_header && IS_JFIF_MARKER(marker)) ||
             (dstinfo->write_Adobe_marker && IS_ADOBE_MARKER(marker))))
            continue;

        jpeg_write_marker(dstinfo, marker->marker, marker->data, marker->data_length);
    }
}
