#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>

#include "types.h" // Contains user defined types


typedef struct _DecodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    uint file_extn_size;
    char *file_extn;
    char *file_data;
    uint sec_file_size;

    /* Magic String */
    char* magic_string_data;
    uint magic_string_len;
    

    /* Output File Info */
    char *output_fname;
    FILE *fptr_output_file;


} DecodeInfo;


/* decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* skip bmp image header*/
Status skip_bmp_header(FILE* fptr_stego_image);

/* To decode magic string length*/
Status decode_magic_string_length(DecodeInfo *decInfo);

/* Get Magic string*/
Status decode_magic_string(DecodeInfo *encInfo);

/* Decode the secret file extension*/
Status decode_secret_file_extn_size(DecodeInfo * decInfo);

/* decode secret file size*/
Status decode_secret_file_extn_data(DecodeInfo * decInfo);

/* decode secret file data*/
Status decode_secret_file_data_size(DecodeInfo * decInfo);

/* decode secret file data*/
Status decode_secret_file_data(DecodeInfo * decInfo);

/* decode lsb to byte*/
Status decode_lsb_to_byte(char *data,char buffer[],DecodeInfo *decInfo);

/* decode lsb to int*/
Status decode_lsb_to_int(uint *data,char buffer[], DecodeInfo *decInfo);

#endif