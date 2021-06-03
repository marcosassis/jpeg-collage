/* this function is an excerpt from libjpeg original code (transupp.c):
 * 
 * Copyright (C) 1997-2019, Thomas G. Lane, Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IFG-README file. */

#include "libjpeg_wrapper_exif.h"

#define IS_EXIF_MARKER(marker_list) (marker_list != NULL &&                     \
                                     marker_list->marker == JPEG_APP0 + 1 &&    \
                                     marker_list->data_length >= 6 &&           \
                                     GETJOCTET(marker_list->data[0]) == 0x45 && \
                                     GETJOCTET(marker_list->data[1]) == 0x78 && \
                                     GETJOCTET(marker_list->data[2]) == 0x69 && \
                                     GETJOCTET(marker_list->data[3]) == 0x66 && \
                                     GETJOCTET(marker_list->data[4]) == 0 &&    \
                                     GETJOCTET(marker_list->data[5]) == 0)

void libjpeg_adjust_exif_dimensions(jpeg_saved_marker_ptr markers, j_compress_ptr dst_info)
{
    boolean is_motorola; /* Flag for byte order */
    unsigned int number_of_tags, tagnum;
    unsigned int firstoffset, offset;
    JDIMENSION new_value;
    JOCTET *data;
    unsigned int length;

    if (!IS_EXIF_MARKER(markers))
        return;

    data = markers->data + 6;
    length = markers->data_length - 6;

    if (length < 12)
        return; /* Length of an IFD entry */

    dst_info->write_JFIF_header = FALSE;

    /* Discover byte order */
    if (GETJOCTET(data[0]) == 0x49 && GETJOCTET(data[1]) == 0x49)
        is_motorola = FALSE;
    else if (GETJOCTET(data[0]) == 0x4D && GETJOCTET(data[1]) == 0x4D)
        is_motorola = TRUE;
    else
        return;

    /* Check Tag Mark */
    if (is_motorola)
    {
        if (GETJOCTET(data[2]) != 0)
            return;
        if (GETJOCTET(data[3]) != 0x2A)
            return;
    }
    else
    {
        if (GETJOCTET(data[3]) != 0)
            return;
        if (GETJOCTET(data[2]) != 0x2A)
            return;
    }

    /* Get first IFD offset (offset to IFD0) */
    if (is_motorola)
    {
        if (GETJOCTET(data[4]) != 0)
            return;
        if (GETJOCTET(data[5]) != 0)
            return;
        firstoffset = GETJOCTET(data[6]);
        firstoffset <<= 8;
        firstoffset += GETJOCTET(data[7]);
    }
    else
    {
        if (GETJOCTET(data[7]) != 0)
            return;
        if (GETJOCTET(data[6]) != 0)
            return;
        firstoffset = GETJOCTET(data[5]);
        firstoffset <<= 8;
        firstoffset += GETJOCTET(data[4]);
    }
    if (firstoffset > length - 2)
        return; /* check end of data segment */

    /* Get the number of directory entries contained in this IFD */
    if (is_motorola)
    {
        number_of_tags = GETJOCTET(data[firstoffset]);
        number_of_tags <<= 8;
        number_of_tags += GETJOCTET(data[firstoffset + 1]);
    }
    else
    {
        number_of_tags = GETJOCTET(data[firstoffset + 1]);
        number_of_tags <<= 8;
        number_of_tags += GETJOCTET(data[firstoffset]);
    }
    if (number_of_tags == 0)
        return;
    firstoffset += 2;

    /* Search for ExifSubIFD offset Tag in IFD0 */
    for (;;)
    {
        if (firstoffset > length - 12)
            return; /* check end of data segment */
        /* Get Tag number */
        if (is_motorola)
        {
            tagnum = GETJOCTET(data[firstoffset]);
            tagnum <<= 8;
            tagnum += GETJOCTET(data[firstoffset + 1]);
        }
        else
        {
            tagnum = GETJOCTET(data[firstoffset + 1]);
            tagnum <<= 8;
            tagnum += GETJOCTET(data[firstoffset]);
        }
        if (tagnum == 0x8769)
            break; /* found ExifSubIFD offset Tag */
        if (--number_of_tags == 0)
            return;
        firstoffset += 12;
    }

    /* Get the ExifSubIFD offset */
    if (is_motorola)
    {
        if (GETJOCTET(data[firstoffset + 8]) != 0)
            return;
        if (GETJOCTET(data[firstoffset + 9]) != 0)
            return;
        offset = GETJOCTET(data[firstoffset + 10]);
        offset <<= 8;
        offset += GETJOCTET(data[firstoffset + 11]);
    }
    else
    {
        if (GETJOCTET(data[firstoffset + 11]) != 0)
            return;
        if (GETJOCTET(data[firstoffset + 10]) != 0)
            return;
        offset = GETJOCTET(data[firstoffset + 9]);
        offset <<= 8;
        offset += GETJOCTET(data[firstoffset + 8]);
    }
    if (offset > length - 2)
        return; /* check end of data segment */

    /* Get the number of directory entries contained in this SubIFD */
    if (is_motorola)
    {
        number_of_tags = GETJOCTET(data[offset]);
        number_of_tags <<= 8;
        number_of_tags += GETJOCTET(data[offset + 1]);
    }
    else
    {
        number_of_tags = GETJOCTET(data[offset + 1]);
        number_of_tags <<= 8;
        number_of_tags += GETJOCTET(data[offset]);
    }
    if (number_of_tags < 2)
        return;
    offset += 2;

    /* Search for ExifImageWidth and ExifImageHeight Tags in this SubIFD */
    do
    {
        if (offset > length - 12)
            return; /* check end of data segment */
        /* Get Tag number */
        if (is_motorola)
        {
            tagnum = GETJOCTET(data[offset]);
            tagnum <<= 8;
            tagnum += GETJOCTET(data[offset + 1]);
        }
        else
        {
            tagnum = GETJOCTET(data[offset + 1]);
            tagnum <<= 8;
            tagnum += GETJOCTET(data[offset]);
        }
        if (tagnum == 0xA002 || tagnum == 0xA003)
        {
            if (tagnum == 0xA002)
                new_value = dst_info->jpeg_width; /* ExifImageWidth Tag */
            else
                new_value = dst_info->jpeg_height; /* ExifImageHeight Tag */
            if (is_motorola)
            {
                data[offset + 2] = 0; /* Format = unsigned long (4 octets) */
                data[offset + 3] = 4;
                data[offset + 4] = 0; /* Number Of Components = 1 */
                data[offset + 5] = 0;
                data[offset + 6] = 0;
                data[offset + 7] = 1;
                data[offset + 8] = 0;
                data[offset + 9] = 0;
                data[offset + 10] = (JOCTET)((new_value >> 8) & 0xFF);
                data[offset + 11] = (JOCTET)(new_value & 0xFF);
            }
            else
            {
                data[offset + 2] = 4; /* Format = unsigned long (4 octets) */
                data[offset + 3] = 0;
                data[offset + 4] = 1; /* Number Of Components = 1 */
                data[offset + 5] = 0;
                data[offset + 6] = 0;
                data[offset + 7] = 0;
                data[offset + 8] = (JOCTET)(new_value & 0xFF);
                data[offset + 9] = (JOCTET)((new_value >> 8) & 0xFF);
                data[offset + 10] = 0;
                data[offset + 11] = 0;
            }
        }
        offset += 12;
    } while (--number_of_tags);
}