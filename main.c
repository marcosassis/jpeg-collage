#include "libjpeg_wrapper.h"

int main()
{
    libjpeg_src_struct src = {};
    libjpeg_dst_struct dst = {};

    libjpeg_read_src(&src, "a2.jpg", 3, TRUE);

    printf("\n\n%d %d \n%d %d\n\n",
           src.info.X_density, src.info.Y_density,
           src.info.image_width, src.info.image_height);

    printf("\n\n%d %d \n%d %d\n\n",
           src.info.max_h_samp_factor, src.info.min_DCT_h_scaled_size,
           src.info.max_v_samp_factor, src.info.min_DCT_v_scaled_size);

    libjpeg_prepare_dst_like_src(&src, &dst, 555, 555, "d2.jpg", TRUE, TRUE);

    libjpeg_copy_region(&src, &dst, 333, 333, 333, 555, 111, 111);

    libjpeg_finish_src(&src);
    libjpeg_finish_dst(&dst);
}