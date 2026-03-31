/*Name: Sammed kuranagi
Date: 25-02-2026
Description: This file contains all the functions required for decode a secret file into a BMP image using the Least Significant Bit (LSB) technique.
*/

#include <stdio.h>              // Standard input output functions
#include <string.h>             // String handling functions
#include "types.h"              // Status enum and typedefs
#include "common.h"             // Contains MAGIC_STRING
#include <unistd.h>             // For usleep function
#include "decode.h"             // Decode structure and prototypes

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)   // Validate decode arguments
{
    char *check_src_file = strstr(argv[2],".bmp");   // Check if source file contains .bmp

    if(check_src_file == NULL)                       // If not bmp file
    {
        printf("Error : Invalid source file\n");     // Print error
        return e_failure;                            // Return failure
    }

    strcpy(decInfo -> encoded_file_name, argv[2]);   // Copy encoded file name into structure

    if (argv[3] == NULL)                             // If output file not provided
    {
        strcpy(decInfo->secret_text_fname, "output"); // Set default name as "output"
    }

    else                                             // If output name is provided
    {
        char *check_output_file = strchr(argv[3],'.');  // Check if extension exists

        if(check_output_file == NULL)                   // If no extension
        {
            strcpy(decInfo ->  secret_text_fname,"output"); // Use default name
        }
        else                                            // If extension exists
        {
            *check_output_file = '\0';                  // Remove extension part
            strcpy(decInfo ->  secret_text_fname,argv[3]); // Copy filename without extension
        }
    }
    return e_success;                                   // Return success
}

Status do_decoding(DecodeInfo *decInfo)                 // Main decoding function
{
    if((open_decode_files(decInfo)) == e_failure)       // Open stego bmp file
        return e_failure;                               // Return failure if error

    skip_bmp_header(decInfo -> fptr_stego_bmp);         // Skip first 54 bytes of BMP header

    if((decode_magic_string(decInfo)) == e_failure)     // Decode magic string
        return e_failure;  

    if((decode_extn_size(decInfo)) == e_failure)        // Decode extension size
        return e_failure;

    if((decode_secret_file_extn(decInfo)) == e_failure) // Decode extension
        return e_failure;

    if((open_secret_file(decInfo)) == e_failure)        // Open output secret file
        return e_failure;

    if((decode_secret_file_size(decInfo)) == e_failure) // Decode secret file size
        return e_failure;

    if((decode_secret_file_data(decInfo)) == e_failure) // Decode secret data
        return e_failure;

    if((write_decoded_data_to_secretfile(decInfo)) == e_failure) // Write decoded data
        return e_failure;

    for(int i = 0; i <= 100; i++)                       // Show progress
    {
        printf("Decoding..%d%%\r",i);                   // Print percentage
        fflush(stdout);                                 // Flush output buffer
        usleep(5000);                                   // Small delay
    }

    printf("\rDecoded successfully\n");                 // Print success message

    return e_success;                                   // Return success
}

Status open_decode_files(DecodeInfo *decInfo)           // Open encoded BMP file
{
    decInfo -> fptr_stego_bmp = fopen(decInfo -> encoded_file_name,"r"); // Open file

    if( decInfo -> fptr_stego_bmp == NULL)              // Check if file opened
    {
        printf("Source file not found\n");              // Print error
        return e_failure;                               // Return failure
    }

    return e_success;                                   // Return success
}

Status skip_bmp_header(FILE *fptr_stego_bmp)            // Skip BMP header
{
    fseek(fptr_stego_bmp,54,SEEK_SET);                  // Move file pointer to 54th byte
    return e_success;                                   // Return success
}

Status decode_magic_string(DecodeInfo *decInfo)         // Decode magic string
{
    int size = strlen(MAGIC_STRING);                    // Get magic string length
    char arr[8];                                        // Buffer to store 8 bytes

    for(int i = 0; i < size; i++)                       // Loop for each character
    {
        fread(arr,8,1,decInfo -> fptr_stego_bmp);       // Read 8 bytes
        decInfo -> magic_string[i] = decode_data_from_lsb(arr); // Decode character
    }

    decInfo -> magic_string[size] = '\0';               // Null terminate string

    if((strcmp(decInfo -> magic_string,MAGIC_STRING)) != 0) // Compare with original
    {
        printf("Error: Invalid magic string\n");        // Print error
        return e_failure;                               // Return failure
    }

    return e_success;                                   // Return success
}

char decode_data_from_lsb(char *arr)                    // Decode one character
{
    char ch = 0;                                        // Initialize character
    
    for(int i = 0; i < 8; i++)                           // Loop 8 bits
    {
        char get = (arr[i] & 1) << i;                    // Extract LSB and shift
        ch = ch | get;                                   // Combine bits
    }

    return ch;                                           // Return decoded character
}

Status decode_extn_size(DecodeInfo *decInfo)            // Decode extension size
{
    char arr[32];                                        // 32 byte buffer
    fread(arr,32,1,decInfo -> fptr_stego_bmp);           // Read 32 bytes
    decInfo -> extn_file_size = decode_integer_to_lsb(arr); // Decode integer

    return e_success;                                    // Return success
}

int decode_integer_to_lsb(char *arr)                     // Decode 32-bit integer
{
    int len = 0;                                         // Initialize length

    for(int i = 0; i < 32; i++)                           // Loop 32 bits
    {
        char get = (arr[i] & 1) << i;                     // Extract bit
        len = len | get;                                  // Combine bits
    }

    return len;                                           // Return integer
}

Status decode_secret_file_extn(DecodeInfo *decInfo)      // Decode file extension
{
    char arr[8];                                          // 8 byte buffer

    for(int i = 0; i < decInfo -> extn_file_size; i++)    // Loop extension size
    {
        fread(arr,8,1,decInfo -> fptr_stego_bmp);         // Read 8 bytes
        decInfo -> decoded_file_extn[i] = decode_data_from_lsb(arr); // Decode char
    }

    decInfo -> decoded_file_extn[decInfo -> extn_file_size] = '\0'; // Null terminate
    return e_success;                                     // Return success
}

Status open_secret_file(DecodeInfo *decInfo)              // Open output secret file
{
    strcat(decInfo -> secret_text_fname,  decInfo -> decoded_file_extn); // Append extension

    decInfo -> fptr_secret_data = fopen(decInfo -> secret_text_fname,"w"); // Open file

    if((decInfo -> fptr_secret_data) == NULL)              // Check error
    {
        printf("Error : secret file not found\n");         // Print error
        return e_failure;                                  // Return failure
    }

    return e_success;                                      // Return success
}

Status decode_secret_file_size(DecodeInfo *decInfo)        // Decode secret file size
{
    char arr[32];                                           // 32 byte buffer
    fread(arr,32,1,decInfo -> fptr_stego_bmp);              // Read 32 bytes
    decInfo -> secret_data_size = decode_integer_to_lsb(arr); // Decode size

    return e_success;                                       // Return success
}

Status decode_secret_file_data(DecodeInfo *decInfo)         // Decode secret data
{
    int size = decInfo -> secret_data_size;                 // Get secret size
    char arr[8];                                            // 8 byte buffer

    for(int i = 0; i < size; i++)                           // Loop each character
    {
        fread(arr,8,1,decInfo -> fptr_stego_bmp);           // Read 8 bytes
        decInfo -> decoded_secret_data[i] = decode_data_from_lsb(arr); // Decode char
    }

    decInfo -> decoded_secret_data[size] = '\0';            // Null terminate
    return e_success;                                       // Return success
}

Status write_decoded_data_to_secretfile(DecodeInfo *decInfo) // Write decoded data
{
    fputs(decInfo -> decoded_secret_data,decInfo -> fptr_secret_data); // Write to file
    return e_success;                                       // Return success
}