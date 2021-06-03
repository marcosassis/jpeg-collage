#include "libjpeg_wrapper.h"
#include "libjpeg_wrapper_markers.h"
#include "libjpeg_wrapper_exif.h"
#include "libjpeg_wrapper_getters.h"

void libjpeg_prepare_dst_like_src(libjpeg_src_struct *src, libjpeg_dst_struct *dst,
                                  JDIMENSION dst_width, JDIMENSION dst_height, char *out_path,
                                  boolean progressive, boolean copy_markers)
{

    libjpeg_setup_dst(dst, 3);
    dst->coef_arrays = libjpeg_request_on_dst(&src->info, &dst->info, dst_width, dst_height);
    libjpeg_copy_core(&src->info, &dst->info, dst_width, dst_height);
    libjpeg_open_dst_file(dst, out_path);
    if (progressive)
        jpeg_simple_progression(&dst->info);
    jpeg_write_coefficients(&dst->info, dst->coef_arrays);
    if (copy_markers)
        libjpeg_copy_markers_execute(&src->info, &dst->info);
}





void libjpeg_setup_std_error(libjpeg_common_struct *img, int trace_level)
{
    img->info.err = jpeg_std_error(&img->err_mgr);
    img->err_mgr.trace_level = trace_level;
}

void libjpeg_open_file(libjpeg_common_struct *img, char *file_path, char *mode)
{
    if ((img->fp = fopen(file_path, mode)) == NULL)
    {
        fprintf(stderr, "can't open \"%s\" on mode \"%s\"\n", file_path, mode);
        exit(EXIT_FAILURE);
    }
}

void libjpeg_open_src_file(libjpeg_src_struct *src, char *file_path)
{
    if (file_path == NULL)
        src->fp = stdin;
    else
        libjpeg_open_file((libjpeg_common_struct *)src, file_path, "rb");

    jpeg_stdio_src(&src->info, src->fp);
}

void libjpeg_open_dst_file(libjpeg_dst_struct *dst, char *file_path)
{
    if (file_path == NULL)
        dst->fp = stdout;
    else
        libjpeg_open_file((libjpeg_common_struct *)dst, file_path, "wb");

    jpeg_stdio_dest(&dst->info, dst->fp);
}

void libjpeg_setup_src(libjpeg_src_struct *src, int trace_level)
{
    libjpeg_setup_std_error((libjpeg_common_struct *)src, 3);
    jpeg_create_decompress(&src->info);
}

void libjpeg_setup_dst(libjpeg_dst_struct *dst, int trace_level)
{
    libjpeg_setup_std_error((libjpeg_common_struct *)dst, 3);
    jpeg_create_compress(&dst->info);
}

void libjpeg_read_src_core(j_decompress_ptr src_info)
{
    (void)jpeg_read_header(src_info, TRUE);
    jpeg_core_output_dimensions(src_info);
}

void libjpeg_read_src(libjpeg_src_struct *src, char *input_path,
                      int trace_level, boolean copy_markers)
{
    libjpeg_setup_src(src, trace_level);
    libjpeg_open_src_file(src, input_path);
    if (copy_markers)
        libjpeg_copy_markers_setup(&src->info, TRUE);
    libjpeg_read_src_core(&src->info);
    /* dst.coef_arrays = libjpeg_request_on_src(W, H, &src->info); */
    src->coef_arrays = jpeg_read_coefficients(&src->info);
    if (src->fp != stdin)
        fclose(src->fp);
}

void libjpeg_copy_core(j_decompress_ptr src_info, j_compress_ptr dst_info,
                       JDIMENSION w, JDIMENSION h)
{
    jpeg_copy_critical_parameters(src_info, dst_info);

    if (src_info->num_components == 1)
    {
        dst_info->comp_info[0].h_samp_factor = 1; /* force to 1x1 */
        dst_info->comp_info[0].v_samp_factor = 1;
    }

    dst_info->jpeg_width = w;
    dst_info->jpeg_height = h;

    libjpeg_adjust_exif_dimensions(src_info->marker_list, dst_info);
}

void libjpeg_finish_src(libjpeg_src_struct *src)
{
    (void)jpeg_finish_decompress(&src->info);
    jpeg_destroy_decompress(&src->info);
}

void libjpeg_finish_dst(libjpeg_dst_struct *dst)
{
    jpeg_finish_compress(&dst->info);
    jpeg_destroy_compress(&dst->info);
    if (dst->fp != stdout)
        fclose(dst->fp);
}

/* 














 */
jvirt_barray_ptr *libjpeg_alloc_coef_arrays(j_common_ptr info, int num_components)
{
    size_t s = sizeof(jvirt_barray_ptr) * num_components;
    void *ret = (*info->mem->alloc_small)(info, JPOOL_IMAGE, s);
    return (jvirt_barray_ptr *)ret;
}

jvirt_barray_ptr libjpeg_request_blocks(
    j_common_ptr info, boolean pre_zero,
    JDIMENSION blocksperrow, JDIMENSION numrows, JDIMENSION maxaccess)
{
    return (*info->mem->request_virt_barray)(info, JPOOL_IMAGE, pre_zero,
                                             blocksperrow, numrows, maxaccess);
}

jvirt_barray_ptr libjpeg_request_component(j_common_ptr info, int h_samp_factor, int v_samp_factor,
                                           JDIMENSION width_in_iMCUs, JDIMENSION height_in_iMCUs)
{
    JDIMENSION width_in_blocks = width_in_iMCUs * h_samp_factor;
    JDIMENSION height_in_blocks = height_in_iMCUs * v_samp_factor;
    return libjpeg_request_blocks(info, TRUE, width_in_blocks, height_in_blocks, v_samp_factor);
}

/* 




















 */

jvirt_barray_ptr *libjpeg_request_like_src(j_decompress_ptr src_info, j_common_ptr mem_info,
                                           JDIMENSION w, JDIMENSION h)
{
    int num_components = src_info->num_components;
    jvirt_barray_ptr *coef_arrays = libjpeg_alloc_coef_arrays(mem_info, num_components);
    JDIMENSION width_in_iMCUs = LIBJPEG_in_iMUCs_horizontal(src_info, w);
    JDIMENSION height_in_iMCUs = LIBJPEG_in_iMUCs_vertical(src_info, h);
    jpeg_component_info *comp_info = src_info->comp_info;

    int ci, h_samp_factor, v_samp_factor;
    for (ci = 0; ci < num_components; ci++, comp_info++)
    {
        if (num_components == 1)
        {
            h_samp_factor = v_samp_factor = 1; /* force to 1x1 */
        }
        else
        {
            h_samp_factor = comp_info->h_samp_factor;
            v_samp_factor = comp_info->v_samp_factor;
        }
        coef_arrays[ci] = libjpeg_request_component(mem_info, h_samp_factor, v_samp_factor,
                                                    width_in_iMCUs, height_in_iMCUs);
    }
    return coef_arrays;
}

jvirt_barray_ptr *libjpeg_request_on_dst(j_decompress_ptr src_info, j_compress_ptr dst_info,
                                         JDIMENSION w, JDIMENSION h)
{
    return libjpeg_request_like_src(src_info, (j_common_ptr)dst_info, w, h);
}

/* must be called after jpeg_read_header (which reads the image dimensions)
 * and before jpeg_read_coefficients (which realizes the source's virtual arrays). */
jvirt_barray_ptr *libjpeg_request_on_src(j_decompress_ptr src_info,
                                         JDIMENSION w, JDIMENSION h)
{
    return libjpeg_request_like_src(src_info, (j_common_ptr)src_info, w, h);
}

/*  */
/*  */
/*  */

JBLOCKARRAY libjpeg_get_strip(j_common_ptr cinfo, jvirt_barray_ptr ptr,
                              JDIMENSION start_row, JDIMENSION num_rows, boolean writable)
{
    return (*cinfo->mem->access_virt_barray)(cinfo, ptr, start_row, num_rows, writable);
}

void jcopy_block_row(JBLOCKROW input_row, JBLOCKROW output_row, JDIMENSION num_blocks);

void libjpeg_copy_region(libjpeg_src_struct *src, libjpeg_dst_struct *dst,
                         JDIMENSION w, JDIMENSION h,
                         JDIMENSION in_off_x, JDIMENSION in_off_y,
                         JDIMENSION out_off_x, JDIMENSION out_off_y)
{
    j_decompress_ptr src_info = &src->info;
    j_compress_ptr dst_info = &dst->info;
    JDIMENSION w_iMCUs = LIBJPEG_in_iMUCs_horizontal(src_info, w);
    JDIMENSION h_iMCUs = LIBJPEG_in_iMUCs_vertical(src_info, h);
    JDIMENSION xin_iMCUs = LIBJPEG_in_iMUCs_horizontal(src_info, in_off_x);
    JDIMENSION yin_iMCUs = LIBJPEG_in_iMUCs_vertical(src_info, in_off_y);
    JDIMENSION xout_iMCUs = LIBJPEG_in_iMUCs_horizontal(src_info, out_off_x);
    JDIMENSION yout_iMCUs = LIBJPEG_in_iMUCs_vertical(src_info, out_off_y);
    JDIMENSION w_blocks, h_blocks, xin_blocks, yin_blocks, xout_blocks, yout_blocks;
    JDIMENSION vsf, hsf, start_row, dst_blk_x;
    jpeg_component_info *compptr = dst_info->comp_info;
    /* compptr->height_in_blocks */
    /* compptr->width_in_blocks */
    JBLOCKARRAY src_buffer, dst_buffer;
    int ci, offset_y;
    JCOEF dc;
    for (ci = 0; ci < dst_info->num_components; ci++, compptr++)
    {
        vsf = compptr->v_samp_factor;
        hsf = compptr->h_samp_factor;
        w_blocks = w_iMCUs * hsf;
        h_blocks = h_iMCUs * vsf;
        xin_blocks = xin_iMCUs * hsf;
        yin_blocks = yin_iMCUs * vsf;
        xout_blocks = xout_iMCUs * hsf;
        yout_blocks = yout_iMCUs * vsf;

        for (start_row = 0; start_row < yout_blocks; start_row += vsf)
        {
            src_buffer = libjpeg_get_strip((j_common_ptr)src_info, src->coef_arrays[ci],
                                           yin_blocks + yout_blocks - start_row,
                                           vsf, FALSE);
            dst_buffer = libjpeg_get_strip((j_common_ptr)dst_info, dst->coef_arrays[ci],
                                           start_row, vsf, TRUE);
            for (offset_y = 0; offset_y < vsf; offset_y++)
            {
                if (xout_blocks > 0)
                {
                    dc = src_buffer[offset_y][xin_blocks][0];
                    for (dst_blk_x = 0; dst_blk_x < xout_blocks; dst_blk_x++)
                    {
                        dst_buffer[offset_y][dst_blk_x][0] = dst_blk_x * 99;
                    }
                }
                jcopy_block_row(src_buffer[offset_y] + xin_blocks,
                                dst_buffer[offset_y] + xout_blocks,
                                w_blocks);
            }
        }

        for (start_row = 0; start_row < h_blocks; start_row += vsf)
        {
            src_buffer = libjpeg_get_strip((j_common_ptr)src_info, src->coef_arrays[ci],
                                           start_row + yin_blocks, vsf, FALSE);
            dst_buffer = libjpeg_get_strip((j_common_ptr)dst_info, dst->coef_arrays[ci],
                                           start_row + yout_blocks, vsf, TRUE);
            for (offset_y = 0; offset_y < vsf; offset_y++)
            {
                for (offset_y = 0; offset_y < vsf; offset_y++)
                {
                    if (xout_blocks > 0)
                    {
                        dc = src_buffer[offset_y][xin_blocks][0];
                        for (dst_blk_x = 0; dst_blk_x < xout_blocks; dst_blk_x++)
                        {
                            dst_buffer[offset_y][dst_blk_x][0] = dc;
                        }
                    }
                    jcopy_block_row(src_buffer[offset_y] + xin_blocks,
                                    dst_buffer[offset_y] + xout_blocks,
                                    w_blocks);
                }
                jcopy_block_row(src_buffer[offset_y] + xin_blocks,
                                dst_buffer[offset_y] + xout_blocks,
                                w_blocks);
            }
        }
    }
    dst_buffer = libjpeg_get_strip((j_common_ptr)dst_info, dst->coef_arrays[1], 0, vsf, TRUE);
    for (dst_blk_x = 0; dst_blk_x < xout_blocks * 4; dst_blk_x++)
    {
        dst_buffer[15][dst_blk_x][2] = 999;
    }
}

/*  */
/*  */
/*  */

void jcopy_sample_rows(JSAMPARRAY input_array, int source_row,
                       JSAMPARRAY output_array, int dest_row, int num_rows,
                       JDIMENSION num_cols);