#ifndef LIBJPEG_WRAPPER_H
#define LIBJPEG_WRAPPER_H
#include "libjpeg_wrapper_include.h"

#define LIBJPEG_AUX_FIELDS          \
    FILE *fp;                       \
    jvirt_barray_ptr *coef_arrays;  \
    struct jpeg_error_mgr err_mgr;  \
    JDIMENSION width;               \
    JDIMENSION height;              \
    JDIMENSION width_in_iMCUs;      \
    JDIMENSION height_in_iMCUs;     \
    int num_components

typedef struct
{
    LIBJPEG_AUX_FIELDS;
    struct jpeg_common_struct info;
} libjpeg_common_struct;

typedef struct
{
    LIBJPEG_AUX_FIELDS;
    struct jpeg_decompress_struct info;
} libjpeg_src_struct;

typedef struct
{
    LIBJPEG_AUX_FIELDS;
    struct jpeg_compress_struct info;
} libjpeg_dst_struct;

/* 
typedef struct
{
    FILE *fp;
    jvirt_barray_ptr *coef_arrays;
    struct jpeg_error_mgr err_mgr;
    struct jpeg_common_struct info;
} libjpeg_common_struct;

typedef struct
{
    FILE *fp;
    jvirt_barray_ptr *coef_arrays;
    struct jpeg_error_mgr err_mgr;
    struct jpeg_decompress_struct info;
} libjpeg_src_struct;

typedef struct
{
    FILE *fp;
    jvirt_barray_ptr *coef_arrays;
    struct jpeg_error_mgr err_mgr;
    struct jpeg_compress_struct info;
} libjpeg_dst_struct;
*/

void libjpeg_open_src_file(libjpeg_src_struct *src, char *file_path);
void libjpeg_open_dst_file(libjpeg_dst_struct *dst, char *file_path);

void libjpeg_setup_src(libjpeg_src_struct *src, int trace_level);
void libjpeg_setup_dst(libjpeg_dst_struct *dst, int trace_level);

void libjpeg_read_src_core(j_decompress_ptr src_info);

void libjpeg_read_src(libjpeg_src_struct *src, char *input_path,
                      int trace_level, boolean copy_markers);

void libjpeg_copy_core(j_decompress_ptr src_info, j_compress_ptr dst_info,
                       JDIMENSION dst_width, JDIMENSION dst_height);

void libjpeg_prepare_dst_like_src(libjpeg_src_struct *src, libjpeg_dst_struct *dst,
                                  JDIMENSION dst_width, JDIMENSION dst_height, char *out_path,
                                  boolean progressive, boolean copy_markers);

/* must be called after jpeg_read_header (which reads the image dimensions)
 * and before jpeg_read_coefficients (which realizes the source's virtual arrays). */
jvirt_barray_ptr *libjpeg_request_on_src(j_decompress_ptr src_info, JDIMENSION w, JDIMENSION h);

/* double buffer. can be called after src read */
jvirt_barray_ptr *libjpeg_request_on_dst(j_decompress_ptr src_info, j_compress_ptr dst_info,
                                         JDIMENSION w, JDIMENSION h);

/* must be called after jpeg_read_header (which reads the image dimensions)
 * and before jpeg_read_coefficients (which realizes the source's virtual arrays). */

void libjpeg_copy_region(libjpeg_src_struct *src, libjpeg_dst_struct *dst,
                         JDIMENSION w, JDIMENSION h,
                         JDIMENSION in_off_x, JDIMENSION in_off_y,
                         JDIMENSION out_off_x, JDIMENSION out_off_y);

void libjpeg_finish_src(libjpeg_src_struct *src);

void libjpeg_finish_dst(libjpeg_dst_struct *dst);

/* lower level, called by others: */
void libjpeg_setup_std_error(libjpeg_common_struct *img, int trace_level);
void libjpeg_open_file(libjpeg_common_struct *img, char *file_path, char *mode);

/* both libjpeg_request_on_dst and libjpeg_request_on_src just call this appropriately */
jvirt_barray_ptr *libjpeg_request_like_src(j_decompress_ptr src_info, j_common_ptr mem_info,
                                           JDIMENSION w, JDIMENSION h);

jvirt_barray_ptr *libjpeg_alloc_coef_arrays(j_common_ptr info, int num_components);

jvirt_barray_ptr libjpeg_request_blocks(
    j_common_ptr info, boolean pre_zero,
    JDIMENSION blocksperrow, JDIMENSION numrows, JDIMENSION maxaccess);

jvirt_barray_ptr libjpeg_request_component(j_common_ptr info, int h_samp_factor, int v_samp_factor,
                                           JDIMENSION width_in_iMCUs, JDIMENSION height_in_iMCUs);

#endif
