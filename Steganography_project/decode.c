#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"

//Read and Validate files
 Status read_and_validate_decode_args(int argc, char *argv[],DecodeInfo* decInfo) {

    //Check if there are 3 files or not
    if (argc < 3) {
        printf("Needs atleast 3 arguments\n");
        return d_failure;
    }else {
        //If stego image is not having .bmp extension
        if (!(strcasestr(argv[2], ".bmp"))) {
            printf("File should be \".bmp\" extension\n");
            return d_failure;
        }
        if (argv[3] != NULL) {
            //If the output file is given, then store it in structure
            decInfo->output_fname = malloc(strlen(argv[3]));
            strcpy(decInfo->output_fname, argv[3]);
        }else {
            //If the output file is given, store it in structure
            decInfo->output_fname = malloc(strlen("Output"));
            strcpy(decInfo->output_fname, "output");
        }
    }

    //Copying the stego image to structure
    decInfo->stego_image_fname = malloc(strlen(argv[2]) + 1);
    strcpy(decInfo->stego_image_fname, argv[2]);
    return d_success;
 }

 //Opening files
 Status open_files_decode(DecodeInfo* decInfo) {

    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return d_failure;
    }
    return d_success;
 }

 //Skip bmp header
 Status skip_bmp_header(FILE* fptr_stego_image) {

    //Skip 54 bytes from stego image
    fseek(fptr_stego_image, 54, SEEK_SET);
    //Check if both offsets are at same position or not
    if (ftell(fptr_stego_image) != 54) {
        return d_failure;
    }
    return d_success;
}

 /* To decode magic string length*/
Status decode_magic_string_length(DecodeInfo *decInfo) {
    //Initialize magic string length to 0
    decInfo->magic_string_len = 0;
    //Buffer to store 32 bytes of stego image data
    char MagicDataBuff[32];
    //Reading 32 bytes from stego image
    fread(MagicDataBuff, 32, 1, decInfo->fptr_stego_image);
    //Decoding byte by byte
    decode_lsb_to_int(&decInfo->magic_string_len, MagicDataBuff, decInfo);
    //Check if both offsets are at same position or not
    int track = 54 + (sizeof(decInfo->magic_string_len)*8);
    if(ftell(decInfo->fptr_stego_image) != track)
    {
        return d_failure;
    }
    return d_success;
}

/* Get Magic string*/
Status decode_magic_string(DecodeInfo *decInfo) {
    //Allocate memory for magic string in heap
    decInfo->magic_string_data = calloc(decInfo->magic_string_len, sizeof(char));

    //Iterate through magic string length
    for (int i = 0; i < decInfo->magic_string_len; i++) {
        //Buffer to store 8 bytes
        char tempMagicData[8];
        //Reading data from stego image
        fread(tempMagicData, 8, 1, decInfo->fptr_stego_image);
        //Decode byte by byte 
        decode_lsb_to_byte(&decInfo->magic_string_data [i], tempMagicData, decInfo);
    }
    //Check if both offsets are at same position or not
    int track = 54 + (sizeof(decInfo->magic_string_len)*8) + ((decInfo->magic_string_len)*8);
    if (ftell(decInfo->fptr_stego_image) != track){
        return d_failure;
    }
    return d_success;
}


/* Decode the secret file extension*/
Status decode_secret_file_extn_size(DecodeInfo * decInfo) {
    //Initializing file extension size to 0
    decInfo->file_extn_size = 0;
    //Buffer to store 32 byte to decode
    char FileExtnSize[32];
    //Reading 32 bytes from stego image
    fread(FileExtnSize, 32, 1, decInfo->fptr_stego_image);
    //Decode integer(file extension size)
    decode_lsb_to_int(&decInfo->file_extn_size, FileExtnSize, decInfo);
    //Check if both offsets are at same position or not
    int track = 54 + (sizeof(decInfo->magic_string_len)*8) + ((decInfo->magic_string_len)*8) + (sizeof((decInfo->file_extn_size))*8);
    if (ftell(decInfo->fptr_stego_image) != track) {
        return d_failure;
    }
    return d_success;
}


/* decode secret file size*/
Status decode_secret_file_extn_data(DecodeInfo * decInfo) {
    //Allocating memory for file extension in heap
    decInfo->file_extn = calloc(decInfo->file_extn_size, sizeof(char));
    //Buffer to collect 8 bytes of stego image data
    char fileExtBuff[8];
    //Iterate through file extension size times
    for (int i = 0; i < decInfo->file_extn_size; i++) {
        //Reading data from stego image
        fread(fileExtBuff, 8, 1, decInfo->fptr_stego_image);
        //Decoding byte by byte
        decode_lsb_to_byte(&decInfo->file_extn[i], fileExtBuff, decInfo);
    }
    //Check if both offsets are at same position or not
    int track = 54 + (sizeof(decInfo->magic_string_len) * 8) + ((decInfo->magic_string_len) * 8) + (sizeof((decInfo->file_extn_size))*8) + ((decInfo->file_extn_size) * 8);
    if (ftell(decInfo->fptr_stego_image) != track) {
        return d_failure;
    }
    return d_success;
}

/* decode secret file data*/
Status decode_secret_file_data_size(DecodeInfo * decInfo) {
    //Initialize file size to 0
    decInfo->sec_file_size = 0;
    //Buffer to collect 32 bytes
    char SecFileSize[32];
    //Reading 32 bytes from stego image
    fread(SecFileSize, 32, 1, decInfo->fptr_stego_image);
    //Decode each byte 
    decode_lsb_to_int(&decInfo->sec_file_size, SecFileSize, decInfo);
    //Check if both offsets are at same position or not
    int track = 54 + (sizeof(decInfo->magic_string_len) * 8) + ((decInfo->magic_string_len) * 8) + (sizeof((decInfo->file_extn_size))*8) + ((decInfo->file_extn_size) * 8) + (sizeof((decInfo->sec_file_size)) * 8);
    if (ftell(decInfo->fptr_stego_image) != track) {
        return d_failure;
    }
    return d_success;
}


/* decode secret file data*/
Status decode_secret_file_data(DecodeInfo * decInfo) {
    //Allocating memory to file data
    decInfo->file_data = calloc(decInfo->sec_file_size, sizeof(char));
    //Iterate through file size times
    for (int i = 0; i < decInfo->sec_file_size; i++) {
        //Buffer to store 8 bytes
        char secDataBuff[8];
        //Reading 8 bytes from stego image
        fread(secDataBuff, 8, 1, decInfo->fptr_stego_image);
        //Decoding each byte
        decode_lsb_to_byte(&decInfo->file_data[i], secDataBuff, decInfo);
        //Writing it to output file
        putc(decInfo->file_data[i], decInfo->fptr_output_file);
    }

    //Check if both offsets are at same position or not
    int track = 54 + (sizeof(decInfo->magic_string_len) * 8) + ((decInfo->magic_string_len) * 8) + (sizeof(decInfo->file_extn_size)*8) + ((decInfo->file_extn_size) * 8) + (sizeof(decInfo->sec_file_size) * 8) + ((decInfo->sec_file_size) * 8);
    if (ftell(decInfo->fptr_stego_image) != track) {
        return d_failure;
    }
    return d_success;
}

/* decode lsb to byte */
Status decode_lsb_to_byte(char *data,char buffer[],DecodeInfo *decInfo) {
    int j = 0;
    for (int i = 7; i >= 0; i--) {
        //Getting lsb from each byte
        unsigned char lsb = (buffer[j] & 0x01) << i;
        //Merging with number
        *data = *data | lsb;
        j++;
    }
    return d_success;
    
}


/* decode lsb to int*/
Status decode_lsb_to_int(uint *data,char buffer[], DecodeInfo *decInfo) {
    int j = 0;
    for (int i = 31; i >= 0; i--) {
        //Getting lsb from each byte
        uint lsb = (buffer[j] & 0x01) << i;
        //Merging with number
        *data = *data | lsb;
        j++;
    }
    return d_success;
}

//Do decode
Status do_decoding(DecodeInfo* decInfo) {
    //Checking the opening of files
    Status OpeningFiles = open_files_decode(decInfo);
    if (OpeningFiles == d_failure) {
        return d_failure;
    }

    //Skipping header file
    Status SkipHeader = skip_bmp_header(decInfo->fptr_stego_image);
    if (SkipHeader == e_failure) {
        printf("failed to skip bmp header\n");
        return d_failure;
    }

    //Decode magic string length/size
    Status decMagicStrLen = decode_magic_string_length(decInfo);
    if (decMagicStrLen == d_failure) {
        printf("Failed to decode magic string length\n");
        return d_failure;
    }

    //Decode magic string data
    Status decMagicStrData = decode_magic_string(decInfo);
    if (decMagicStrData == d_failure) {
        printf("Failed to decode magic string data\n");
        return d_failure;
    }
    char tempSec[30];
    printf("Enter the secret code: ");
    scanf("%[^\n]", tempSec);

    if (strcmp(decInfo->magic_string_data, tempSec)) {
        printf("Incorrect password\n");
        return d_failure;
    }

    //Decoding secretfile extension size
    Status decFileSize = decode_secret_file_extn_size(decInfo);
    if (decFileSize == d_failure) {
        printf("Failed to decode file extern size\n");
        return d_failure;
    }

    //Decode Secret file extension data
    Status decSecExtdata = decode_secret_file_extn_data(decInfo);
    if (decSecExtdata == d_failure) {
        printf("Failed to decode secret file extension data\n");
        return d_failure;
    }
        // concatinating the extension with file name
        strcat(decInfo->output_fname, decInfo->file_extn);
        //opening the output file
        decInfo->fptr_output_file = fopen(decInfo->output_fname, "w");
        if (decInfo->fptr_output_file == NULL) {

            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);

            return d_failure;
        }

    //Decode Secret file size
    Status decSecFileSize = decode_secret_file_data_size(decInfo);
    if (decSecFileSize == d_failure) {
        printf("Failed to decode secret file size\n");
        return d_failure;
    }

    //Decode Secret file data
    Status decSecFileData = decode_secret_file_data(decInfo);
    if (decSecFileData == d_failure) {
        printf("Failed to decode Secret file data\n");
        return d_failure;
    }

    return d_success;
}