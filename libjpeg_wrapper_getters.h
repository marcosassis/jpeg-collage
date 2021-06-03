#ifndef LIBJPEG_WRAPPER_GETTERS_H
#define LIBJPEG_WRAPPER_GETTERS_H

/* ceil(a/b). assumes a >= 0, b > 0 */
#define LIBJPEG_div_ceil(a, b) \
    ((a + b - 1L) / b)

#define LIBJPEG_iMCU_sample_width(info_ptr) \
    ((info_ptr)->max_h_samp_factor * (info_ptr)->min_DCT_h_scaled_size)

#define LIBJPEG_iMCU_sample_height(info_ptr) \
    ((info_ptr)->max_v_samp_factor * (info_ptr)->min_DCT_v_scaled_size)

#define LIBJPEG_in_iMUCs_horizontal(info_ptr, val) \
    LIBJPEG_div_ceil(val, LIBJPEG_iMCU_sample_width(info_ptr))

#define LIBJPEG_in_iMUCs_vertical(info_ptr, val) \
    LIBJPEG_div_ceil(val, LIBJPEG_iMCU_sample_height(info_ptr))

#define LIBJPEG_num_components(info_ptr) \
    ((info_ptr)->num_components)



#endif
