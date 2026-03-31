/*Name: Sammed kuranagi
Date: 25-02-2026
Description: 
*/

#include <stdio.h>              // Standard input output library
#include <string.h>             // String handling functions
#include "encode.h"             // Encode structure and prototypes
#include "types.h"              // Status and OperationType definitions
#include "decode.h"             // Decode structure and prototypes
#include <stdlib.h>             // For malloc and free

OperationType check_operation_type(char *argv[])   // Function to check operation type
{
    if(argv[1] == NULL)            // If no argument given
        return e_unsupported;      // Return unsupported

    else if((strcmp(argv[1],"-e")) == 0)   // If "-e" is given
        return e_encode;                   // Return encode type

    else if((strcmp(argv[1],"-d")) == 0)   // If "-d" is given
        return e_decode;                   // Return decode type

    else                                  // If invalid argument
        return e_unsupported;              // Return unsupported
}

int main(int argc, char *argv[])           // Main function
{
    int res = check_operation_type(argv);  // Check operation type

    if(res == e_encode)                    // If encode selected
    {
        if(argc < 4)                       // Check minimum arguments
        {
            printf("So give the arguments as below\n");  // Print instruction
            printf("./a.out -e <input.bmp> <secret.txt> [output.bmp]\n");  // Print format
            return e_failure;              // Return failure
        }

        EncodeInfo encInfo;                // Declare encode structure

        if (read_and_validate_encode_args(argv, &encInfo) == e_success)  // Validate args
            do_encoding(&encInfo);         // Call encoding function
        else
            return e_failure;              // Return failure if validation fails

        free(encInfo.secret_data);         // Free allocated memory
        fclose(encInfo.fptr_src_image);    // Close source image
        fclose(encInfo.fptr_secret);       // Close secret file
        fclose(encInfo.fptr_stego_image);  // Close stego image
    }

    else if(res == e_decode)               // If decode selected
    {
        if(argc < 3)                       // Check minimum arguments
        {
            printf("Give the arguments as below\n");  // Print instruction
            printf("./a.out -d <steggo.bmp> [outpt.txt]\n");  // Print format
            return e_failure;              // Return failure
        }

        DecodeInfo decInfo;                // Declare decode structure

        if(read_and_validate_decode_args(argv,&decInfo) == e_success)  // Validate args
            do_decoding(&decInfo);         // Call decoding function
        else
            e_failure;                    // Indicate failure

        fclose(decInfo.fptr_stego_bmp);    // Close stego bmp file
        fclose(decInfo.fptr_secret_data);  // Close decoded file
    }

    else                                   // If invalid argument
    {
        printf("Error: Invalid arguments\n");  // Print error
        return e_failure;                  // Return failure
    }

    return 0;                              // Return success
}