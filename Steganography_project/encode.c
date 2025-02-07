#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}


/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo){
    
    //Check if CLA count is 4 or 5
    if (argc < 4) {
        printf("Needs atleast 4 arguments\n");
        return e_failure;
    }else {

        //Check if the source file is valid or not
        if (!(strcasestr(argv[2], ".bmp"))) {
        printf("File should be \".bmp\" extension\n");
            return e_failure;
        }
        //Check if the secret file is valid or not
        if (!(strcasestr(argv[3], ".txt") || strcasestr(argv[3], ".c") || strcasestr(argv[3], ".mp3"))) {
            printf("File should be \".txt\" or \".c\" or \".bmp\" or \".mp3\" extension\n");
            return e_failure;
        }
        //Check if there is destination file is 
        if (argc < 5 || argv[4] == NULL) {

        //Stego file 
            encInfo->stego_image_fname = malloc(strlen("stego.bmp") + 1);
            strcpy(encInfo->stego_image_fname, "stego.bmp");
                
        }else {
            if (!(strcasestr(argv[4], ".bmp"))) {
                printf("File should be \".bmp\" extension\n");
                return e_failure;
            }else {
                //Allocating memory for destination file
                encInfo->stego_image_fname = malloc(strlen(argv[4]) + 1);
                strcpy(encInfo->stego_image_fname, argv[4]);
            }
        } 
    }
    //Saving file names to structure
    // Allocate memory for the source file name and copy it
    encInfo->src_image_fname = malloc(strlen(argv[2]) + 1);
    strcpy(encInfo->src_image_fname, argv[2]);

    // Allocate memory for the secret file name and copy it
    encInfo->secret_fname = malloc(strlen(argv[3]) + 1);
    strcpy(encInfo->secret_fname, argv[3]);

    return e_success;
}

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo) {
    //Opening files, checking the status
    Status encodeOpenfileStatus = open_files(encInfo);
    if (encodeOpenfileStatus == e_failure) {
        printf("Error occured during opening files\n");
        return e_failure;
    }
    char tempMagicStr[50];
    printf("Enter the magic string: ");
    scanf("%[^\n]", tempMagicStr);
    encInfo->magic_String = malloc(strlen(tempMagicStr) * sizeof(char));

    //Copying the magic string to structure
    // encInfo->magic_String = malloc(strlen(tempMagicStr));
    strcpy(encInfo->magic_String, tempMagicStr);
    encInfo->magic_String = realloc(encInfo->magic_String, strlen(encInfo->magic_String));
    // Call check capacity
    Status checkCapacity = check_capacity(encInfo);
    if (checkCapacity == e_failure) {
        return e_failure;
    }
    //Call copy_bmp_header
    Status copyBmpHeader = copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if (copyBmpHeader == e_failure) {
        printf("Failed to copy source header to destinatio\n");
        return e_failure;
    }
    //Encoding magic String length
    Status magicStrSize = encode_magic_string_size(encInfo);
    if (magicStrSize == e_failure) {
        printf("Failed to encode magic string size to destination\n");
        return e_failure;
    }
    //Encoding magic string
    Status encodeMagicStr = encode_magic_string(encInfo);
    if (encodeMagicStr == e_failure) {
        printf("Failed to encode magic string to destination\n");
        return e_failure;
    }
    //Encoding secret file extension size 
    Status extnFileSize = encode_secret_file_extn_size(encInfo);
    if (extnFileSize == e_failure) {
        printf("Failed to encode secret file extern size\n");
        return e_failure;
    }
    //Encoding secret file extern 
    Status secExtnFile = encode_secret_file_extn(encInfo);
    if (secExtnFile == e_failure) {
        printf("Failed to encode secret file extern\n");
        return e_failure;
    }
    //Encoding secret file size
    Status secFileSize = encode_secret_file_size(encInfo);
    if (secFileSize == e_failure) {
        printf("Failed to encode secret file size\n");
        return e_failure;
    }
    //Encoding secret file data
    Status secFiledata = encode_secret_file_data(encInfo);
    if (secFiledata == e_failure) {
        printf("Failed to encode secret file data\n");
        return e_failure;
    }
    //Copying remaining data from source image to destination file
    Status copyRem = copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if (copyRem == e_failure) {
        printf("Failed to copy remaining data to destination\n");
        return e_failure;
    }
    return e_success;
}

/* check capacity */
Status check_capacity(EncodeInfo *encInfo) {
    //Calculating the capacity of source image
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    //Calculating the magic string size
    encInfo->magic_str_size = strlen(encInfo->magic_String);
    encInfo->magic_str_data_size = sizeof(encInfo->magic_str_size);

    //Calculating secret file extension
    //char* tempExt = strchr(encInfo->secret_fname,'.');
    strcpy(encInfo->extn_secret_file, strchr(encInfo->secret_fname,'.'));
    encInfo->extn_data_size = strlen(encInfo->extn_secret_file);
    encInfo->extn_file_size = sizeof(encInfo->extn_data_size);
    
    //Calculating the secret file size
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    encInfo->file_data_size = sizeof(encInfo->size_secret_file);

    //Calculating the total size
    uint totalSize = ((((encInfo->magic_str_data_size) + (encInfo->magic_str_size)) + ((encInfo->extn_file_size) + (encInfo->extn_data_size) + (encInfo->size_secret_file) + (encInfo->file_data_size)) ) * 8) + 54;
    if (totalSize > encInfo->image_capacity) {
        printf("Insufficient size in source image file\n");
        return e_failure;
    }
    return e_success;
}

/* Get file size */
uint get_file_size(FILE *fptr) {
    if (fptr == NULL) {
        printf("Unable to open file\n");
        return e_failure;
    }
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);

}

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image) {
    rewind(fptr_src_image);
    char headerDataBuff[54];
    fread(headerDataBuff, 54, 1, fptr_src_image);
    fwrite(headerDataBuff, 54, 1, fptr_dest_image);

    //Check if both offsets are at same position or not
    if (ftell(fptr_dest_image) != 54) {
        return e_failure;
    }
    return e_success;
}

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer) {
    int j = 0;
    //Iterating from MSB
    for (int i = 7; i >= 0; i--) {
        //Getting each character data
        uint dataBit = (data & (1 << i)) >> i;
        //Clearing image buffer LSB data
        image_buffer[j] = image_buffer[j] & 0xfe;
        //Merging both character and buffer data
        image_buffer[j] |= dataBit;
        j++;
    }
    return e_success;
}

/* Encode a int into LSB of image data array */
Status encode_int_to_lsb(uint data, char *int_32_byte_buffer) {
    int j = 0;

    //Iterate through 32 bits of data
    for (int i = 31; i >= 0; i--) {
        uint dataBit = (data & (1 << i)) >> i;
        //Clearing image buffer LSB data
        int_32_byte_buffer[j] &= 0xFE;
        //Merging both character and buffer data
        int_32_byte_buffer[j] |= (char)dataBit;
        j++;
    }
    return e_success;
} 

/* Encoding magic string size to destination file */
Status encode_magic_string_size(EncodeInfo *encInfo) {

    //Creating temporary array to store source image data of 32 byte to encode
    char int_buffer[32];
    fread(int_buffer, 32, 1, encInfo->fptr_src_image);

    //Encoding 32 bits of data of magic string size
    encode_int_to_lsb(encInfo->magic_str_size, int_buffer);

    //Storing / transferring the data to destination image
    fwrite(int_buffer, 32, 1, encInfo->fptr_stego_image);

    //Check if both offsets are at same position or not
    if (ftell(encInfo->fptr_stego_image) != (54+32)) {
        return e_failure;
    }
    return e_success;
}

/* Store Magic String to destination file */
Status encode_magic_string(EncodeInfo *encInfo) {
    int i;
    char byte_buffer[8];
    //Iterating through magic string size times
    for (i = 0; i < encInfo->magic_str_size; i++) {
        //Collecting the 8bytes of data from source image to buffer
        fread(byte_buffer, 8, 1, encInfo->fptr_src_image);
        //Encoding each character of magic string to 8 bytes of data
        encode_byte_to_lsb(encInfo->magic_String[i], byte_buffer);
        //Passing the updated data to destination file
        fwrite(byte_buffer, 8, 1, encInfo->fptr_stego_image);
    }
    //Check if both offsets are at same position or not
    if (ftell(encInfo->fptr_stego_image) != (54 + 32 + (strlen(encInfo->magic_String) * 8))) {
        return e_failure;
    }
    return e_success;
}

/* Encode secret file extenstion */
Status encode_secret_file_extn_size(EncodeInfo *encInfo) {
    //Creating buffer to store 32 bytes data of source image
    char int_buffer[32];
    //Reading 32 bytes from source to buffer
    fread(int_buffer, 32, 1, encInfo->fptr_src_image);
    //Encoding extn size
    encode_int_to_lsb(encInfo->extn_data_size, int_buffer);
    //Storing / passing the upgraded buffer to destination file
    fwrite(int_buffer, 32, 1, encInfo->fptr_stego_image);

    //Check if both offsets are at same position or not
    int track = (54 + 32 + (strlen(encInfo->magic_String) * 8) + 32);
    if (ftell(encInfo->fptr_stego_image) != track) {
        return e_failure;
    }
    return e_success;
}


/* Encode secret file extenstion */
Status encode_secret_file_extn(EncodeInfo *encInfo) {
    int i;
    //Creating buffer to collect 8 bytes from source image
    char byte_buffer[8];
    //Iterate through extern size times (for (.txt) -> 4 times)
    for (i = 0; i < encInfo->extn_data_size; i++) {
        //Getting 8 bytes of data from source image
        fread(byte_buffer, 8, 1, encInfo->fptr_src_image);
        //Encoding each character into a byte of source image data
        encode_byte_to_lsb(encInfo->extn_secret_file[i], byte_buffer);
        //Storing / passing upgraded data to destination file
        fwrite(byte_buffer, 8, 1, encInfo->fptr_stego_image);
    }
    //Check if both offsets are at same position or not
    int track = (54+32+(strlen(encInfo->magic_String)*8)+32+(strlen(encInfo->extn_secret_file)*8));
    if (ftell(encInfo->fptr_stego_image) != track) {
        return e_failure;
    }
    return e_success;
}

/* Encode secret file size */
Status encode_secret_file_size(EncodeInfo *encInfo) {
    //Creating buffer to store 32 byte of source image data
    char int_buffer[32];
    //Reading 32 byte of data to buffer 
    fread(int_buffer, 32, 1, encInfo->fptr_src_image);
    //Encoding secret file size to buffer
    encode_int_to_lsb(encInfo->size_secret_file, int_buffer);
    //Storing / passing updated buffer to destination file
    fwrite(int_buffer, 32, 1, encInfo->fptr_stego_image);
    //Check if both offsets are at same position or not
    int track = (54 + 32 + (strlen(encInfo->magic_String) * 8) + 32 + (strlen(encInfo->extn_secret_file) * 8) + 32);
    if (ftell(encInfo->fptr_stego_image) != track) {
        return e_failure;
    }
    return e_success;
}

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo) {
    //Creating buffer to store 8 bytes of source image file data
    rewind(encInfo->fptr_secret);
    encInfo->secret_data = calloc(encInfo->size_secret_file, sizeof(char));
    char byte_buffer[8];
    char ch;
    int i = 0;
    //Storing the secret data
    while ((ch = getc(encInfo->fptr_secret)) != EOF) {
        encInfo->secret_data[i++] = ch; 
    }
    encInfo->secret_data[i] = 0;
    for (i = 0; i < strlen(encInfo->secret_data); i++) {
        //Reading 8 bytes of source image data
        fread(byte_buffer, 8, 1, encInfo->fptr_src_image);
        //Encoding each character into a byte of source image data
        encode_byte_to_lsb(encInfo->secret_data[i], byte_buffer);
        //Storing / passing upgraded data to destination file
        fwrite(byte_buffer, 8, 1, encInfo->fptr_stego_image);
    }

    //Check if both offsets are at same position or not
    int track = (54+32+(strlen(encInfo->magic_String)*8)+32+(strlen(encInfo->extn_secret_file)*8)+32+((encInfo->size_secret_file )* 8));
    if (ftell(encInfo->fptr_stego_image) != track) {
        return e_failure;
    }
    return e_success;
}

// Function to copy remaining image data after encoding the secret message
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest) {
    // Get the current position in the source file (right after encoding)
    long encode_end_position = ftell(fptr_src);

    // Move source file pointer to the end to determine file size
    fseek(fptr_src, 0, SEEK_END);
    long fileSize = ftell(fptr_src);

    // Calculate remaining data size
    long remainingSize = fileSize - encode_end_position;

    // Move back to the position after the encoded data
    fseek(fptr_src, encode_end_position, SEEK_SET);

    // Allocate buffer dynamically
    unsigned char *buffer = (unsigned char *)malloc(remainingSize);

    // Read remaining data from the source file
    fread(buffer, 1, remainingSize, fptr_src);

    // Write remaining data to the destination file
    fwrite(buffer, 1, remainingSize, fptr_dest);
    //Check if both offsets are at same position or not
    if (ftell(fptr_dest) != ftell(fptr_src)) {
        return e_failure;
    }
    return e_success;
}


