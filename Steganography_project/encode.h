#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Magic String from user */
    char *magic_String;
    uint magic_str_size;
    uint magic_str_data_size;

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    uint extn_data_size;
    uint extn_file_size;
    uint file_data_size;
    int size_secret_file;
    char extn_secret_file[MAX_IMAGE_BUF_SIZE];
    char *secret_data;
    

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(int argc, char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Encoding the magic string size */
Status encode_magic_string_size(EncodeInfo *encInfo);

/* Store Magic String */
Status encode_magic_string(EncodeInfo *encInfo);

/* Encode secret file extenstion size */
Status encode_secret_file_extn_size(EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Encode a int into LSB of image data array */
Status encode_int_to_lsb(uint data, char *int_32_byte_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
