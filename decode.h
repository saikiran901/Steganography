#ifndef DECODE_H
#define DECODE_H
#include "common.h"

typedef struct DecodeInfo
{
    /*stego image file*/
    char stego_name[30]; 
    FILE *stego_img;
    char output_secret_fname[30];
    FILE *secret_fname;

    int secret_extn_size;
    char secret_extn[10];

    int secret_file_size;

} DecodeInfo;






/* Read and validate decode args from argv */
Status validate_decode_args(char *argv[], DecodeInfo *decInfo);



/* Perform the decoding */
Status do_decoding(char *argv[], DecodeInfo *decInfo);


Status open_img_file(DecodeInfo *decInfo);

Status skip_bmp_header(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);



/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);





/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode 1 byte from LSB of image data array */
Status decode_1byte_from_lsb(char *data, char *buffer_8);


/* Decode 4byte(int) from LSB of image data array */
Status decode_4byte_from_lsb(int *data, char *buffer_32);






#endif
