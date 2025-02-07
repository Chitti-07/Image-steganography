#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

int main(int argc, char* argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;

    OperationType Operation_type = check_operation_type(argc, argv);
    if (Operation_type == e_encode) {
        //Call encode operation
        Status encode_status = read_and_validate_encode_args(argc, argv, &encInfo);
        //If the encode_status is true then call do_encode
        if (encode_status == e_failure) {
            printf("Encoding is not posible\n");
            return e_failure;
        }else {
            Status doEncodeStatus = do_encoding(&encInfo);
            if (doEncodeStatus == e_failure) {
                printf("Encoding failed\n");
            }else {
                printf("Encoding completed successfully");
            }
        }
    }else if (Operation_type == e_decode) {
        //Call decode operation
        printf("Decode\n");
        Status decode_status = read_and_validate_decode_args(argc, argv, &decInfo);
        //If decode status is true, then call do decode
        if (decode_status == d_failure) {
            printf("Decoding is not possible\n");
            return d_failure;
        }else {
            Status deDecoding = do_decoding(&decInfo);
            if (deDecoding == d_failure) {
                printf("Decoding failed\n");
            }else {
                printf("Decoding completed successfully\n");
            }
        }
    }
    return 0;
}

OperationType check_operation_type(int argc, char *argv[]) {
    //check if it is encode or decode
    if (argc <= 1) {
        printf("Atleast 4 arguments should be passed\n");
        return e_unsupported;
    //Check the opetarion mode (encode / decode)
    }else if (!(strcasecmp(argv[1], "-e"))) {
        return e_encode;
    }else if (!(strcasecmp(argv[1], "-d"))) {
        return e_decode;
    }else {
        printf("Invalid operation type\n");
        return e_unsupported;
    }
}
