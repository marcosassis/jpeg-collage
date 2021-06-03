#ifndef JPEG_LOSSLESS_TRANS_H
#define JPEG_LOSSLESS_TRANS_H

#include "libjpeg_wrapper.h"

typedef struct
{
    JDIMENSION w, h;
} jpeg_trans_dim_t;

typedef struct
{
    JDIMENSION x, y;
} jpeg_trans_pos_t;

typedef struct
{
    jpeg_trans_dim_t size;
    jpeg_trans_pos_t pos;
} jpeg_trans_frame_t;

typedef struct
{
    jpeg_trans_dim_t canvas_size;
    jpeg_trans_frame_t src_drop_frame;
    libjpeg_src_struct src;
    libjpeg_dst_struct dst;
    char *output_path;
    boolean copy_markers;
} jpeg_trans_struct;

void jpeg_setup_trans(jpeg_trans_struct *t, int trace_level)
{
    libjpeg_setup_src(&t->src, 3);
    libjpeg_setup_dst(&t->dst, 3);
}

void jpeg_init_drop_frame(jpeg_trans_struct *t)
{
    t->src_drop_frame.pos.x = 0;
    t->src_drop_frame.pos.y = 0;
    t->src_drop_frame.size.w = t->src.info.image_width;
    t->src_drop_frame.size.h = t->src.info.image_height;
}

void jpeg_init_canvas_size(jpeg_trans_struct *t)
{
    t->canvas_size.w = t->src.info.image_width;
    t->canvas_size.h = t->src.info.image_height;
}

/* you can set t->canvas_size.w and .h before calling this */
void jpeg_prepare_out(jpeg_trans_struct *t)
{
    t->dst.coef_arrays = libjpeg_request_on_dst(t->canvas_size.w,
                                              t->canvas_size.h,
                                              &t->src.info,
                                              &t->dst.info);
}

/* 

typedef struct
{
    JDIMENSION w, h;
} jll_dim_struct;

typedef struct
{
    JDIMENSION x, y;
} jll_pos_struct;

typedef struct
{
    jll_dim_struct size;
    jll_pos_struct pos;
    char *path;
    libjpeg_common_struct *libjpeg_info;
} jll_img_struct;

typedef struct
{
    jll_dim_struct canvas_size;

    libjpeg_src_struct src_info;
    libjpeg_dst_struct dst_info;

    jll_img_struct src;
    jll_img_struct dst;

    int iMCU_sample_width;
    int iMCU_sample_height;
    jvirt_barray_ptr *src_coef_arrays, *dst_coef_arrays;

} jll_trans_struct;
 */
/* 


void setup_codec_structs(trans_struct *ts, int trace_level)
{
    ts->src.info = (j_common_ptr)&ts->src_info;
    ts->dst.info = (j_common_ptr)&ts->dst_info;
    setup_std_error(&ts->src, trace_level);
    setup_std_error(&ts->dst, trace_level);
    jpeg_create_decompress(&ts->src_info);
    jpeg_create_compress(&ts->dst_info);
}

void open_src_file(trans_struct *ts, char *file_path)
{
    if (file_path == NULL)
        ts->src.fp = stdin;
    else
        open_file(&ts->src, file_path, "rb");

    jpeg_stdio_src(&ts->src_info, ts->src.fp);
}

void open_dst_file(trans_struct *ts, char *file_path)
{
    if (file_path == NULL)
        ts->dst.fp = stdout;
    else
        open_file(&ts->src, file_path, "wb");

    jpeg_stdio_dest(&ts->dst_info, ts->dst.fp);
}

void read_src_header(trans_struct *ts)
{
    (void)jpeg_read_header(&ts->src_info, TRUE);
    jpeg_core_output_dimensions(&ts->src_info);
    ts->iMCU_sample_width = ts->src_info.max_h_samp_factor * ts->src_info.min_DCT_h_scaled_size;
    ts->iMCU_sample_height = ts->src_info.max_v_samp_factor * ts->src_info.min_DCT_v_scaled_size;
}

/* ceil(a/b). assumes a >= 0, b > 0 */
/* #define DIV_CEIL(a, b) ((a + b - 1L) / b)

void alloc_space(trans_struct *ts)
{
    int ci;
    jpeg_component_info *compptr;
    JDIMENSION width_in_blocks, height_in_blocks, h_samp_factor, v_samp_factor;

    struct jpeg_memory_mgr *memmgr = ts->src_info.mem;
    int num_comp = ts->src_info.num_components;
    jvirt_barray_ptr *coef_arrays =
        (jvirt_barray_ptr *)(*memmgr->alloc_small)(ts->src.info, JPOOL_IMAGE,
                                                   sizeof(jvirt_barray_ptr) * num_comp);

    JDIMENSION width_in_iMCUs = DIV_CEIL(ts->canvas_size.w, ts->iMCU_sample_width);
    JDIMENSION height_in_iMCUs = DIV_CEIL(ts->canvas_size.h, ts->iMCU_sample_height);

    for (ci = 0; ci < num_comp; ci++)
    {
        compptr = ts->src_info.comp_info + ci;
        if (num_comp == 1)
        {
            h_samp_factor = v_samp_factor = 1; /* force to 1x1 */
/*
        }
        else
        {
            h_samp_factor = compptr->h_samp_factor;
            v_samp_factor = compptr->v_samp_factor;
        }
        width_in_blocks = width_in_iMCUs * h_samp_factor;
        height_in_blocks = height_in_iMCUs * v_samp_factor;

        coef_arrays[ci] =
            (*memmgr->request_virt_barray)(ts->src.info, JPOOL_IMAGE, FALSE,
                                           width_in_blocks, height_in_blocks, v_samp_factor);
    }
    return coef_arrays;
}
 */
#endif
