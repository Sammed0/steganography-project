/*Name:Sammed kuranagi
Date:25-02-2026
Description: This file contains all the functions required for encoding a secret file into a BMP image using the Least Significant Bit (LSB) technique
*/

#include <stdio.h>      // Standard input output header
#include <string.h>     // String handling functions
#include "encode.h"     // User defined encode header
#include "types.h"      // Status enum and typedefs
#include "common.h"     // Contains MAGIC_STRING
#include <unistd.h>     // For usleep function
#include <stdlib.h>     // For malloc

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)   // Validate CLI arguments
{
    if((strstr(argv[2],".bmp")) == NULL)   // Check if source file is .bmp
    {
        printf("Invalid source file\n");   // Print error
        return e_failure;                  // Return failure
    }
    else
    {
        encInfo -> src_image_fname = argv[2];   // Store source image name
    }

    char *scr_file = strstr(argv[3],".");   // Get extension of secret file

    if(scr_file == NULL)                    // If no extension
    {
        printf("Invalid secret file\n");    // Print error
        return e_failure;                   // Return failure
    }

    strcpy(encInfo -> extn_secret_file,scr_file);  // Copy extension
    encInfo->secret_fname = argv[3];               // Store secret file name

    if(argv[4] == NULL)                     // If output file not given
    {
        encInfo -> stego_image_fname = "stego.bmp";  // Default output name
    }
    else
    {
        encInfo -> stego_image_fname = argv[4];      // Store output filename
    }

    return e_success;                       // Return success
}

uint get_image_size_for_bmp(FILE *fptr_image)   // Get BMP capacity
{
    uint width, height;                     // Declare width & height
    fseek(fptr_image, 18, SEEK_SET);        // Move to width location
    fread(&width, sizeof(int), 1, fptr_image);   // Read width
    fread(&height, sizeof(int), 1, fptr_image);  // Read height
    return width * height * 3;              // Return total RGB capacity
}

Status open_files(EncodeInfo *encInfo)      // Open required files
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");  // Open source image

    if (encInfo->fptr_src_image == NULL)    // Check error
    {
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);  // Print error
        return e_failure;                   // Return failure
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");  // Open secret file

    if (encInfo->fptr_secret == NULL)       // Check error
    {
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);  // Print error
        return e_failure;                   // Return failure
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");  // Open output image

    if (encInfo->fptr_stego_image == NULL)  // Check error
    {
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);  // Print error
        return e_failure;                   // Return failure
    }

    return e_success;                       // Return success
}

uint get_file_size(FILE *fptr)              // Get file size
{
    fseek(fptr, 0,SEEK_END);                // Move to end
    long size = ftell(fptr);                // Get position
    rewind(fptr);                           // Reset pointer
    return size;                            // Return size
}

Status check_capacity(EncodeInfo *encInfo)  // Check image capacity
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);  // Get capacity
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);            // Get secret size

    if((encInfo->image_capacity-54) > (strlen(MAGIC_STRING) + sizeof(int) + strlen(encInfo->extn_secret_file) + sizeof(int) + encInfo->size_secret_file )*8)  // Capacity check
        return e_success;                   // Enough space
    else
        return e_failure;                   // Not enough space
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)  // Encode data
{
    char arr[8];                            // Buffer for 8 bytes

    for(int i = 0; i < size; i++)           // Loop for each character
    {
        fread(arr, 8, 1, fptr_src_image);   // Read 8 bytes
        encode_byte_to_lsb(data[i], arr);   // Encode 1 byte
        fwrite(arr, 8, 1, fptr_stego_image);// Write modified bytes
    }

    return e_success;                       // Return success
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)  // Encode magic string
{
    encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo -> fptr_src_image, encInfo -> fptr_stego_image);  // Encode magic
    return e_success;                       // Return success
}

Status do_encoding(EncodeInfo *encInfo)     // Main encoding function
{
    if (open_files(encInfo) == e_failure)   // Open files
        return e_failure;                   // Return failure

    if (check_capacity(encInfo) == e_failure)  // Check capacity
    {
        printf("Secret file size is too large\n");  // Print error
        return e_failure;                   // Return failure
    }

    rewind(encInfo -> fptr_src_image);      // Reset source pointer
    rewind(encInfo -> fptr_secret);         // Reset secret pointer

    if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)  // Copy header
        return e_failure;                   // Return failure

    if(encode_magic_string(MAGIC_STRING,encInfo) == e_failure)  // Encode magic
        return e_failure;                   // Return failure
    
    encInfo -> size_secret_file_extn = strlen(encInfo -> extn_secret_file);  // Get extension size

    if(encode_secret_file_size(encInfo -> size_secret_file_extn, encInfo) == e_failure)  // Encode ext size
        return e_failure;

    if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_failure)  // Encode extension
        return e_failure;

    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);  // Get secret size
    rewind(encInfo -> fptr_secret);           // Reset secret pointer
    encInfo->secret_data = malloc(encInfo->size_secret_file);  // Allocate memory

    if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_failure)  // Encode file size
        return e_failure;

    fread(encInfo -> secret_data,encInfo -> size_secret_file,1,encInfo ->  fptr_secret);  // Read secret data

    if(encode_data_to_image(encInfo -> secret_data,encInfo -> size_secret_file, encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)  // Encode data
        return e_failure;

    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)  // Copy remaining
        return e_failure;
    
    for(int i = 0; i <= 100; i++)           // Progress display loop
    {
        printf("Encoding..%d%%\r",i);       // Print percentage
        fflush(stdout);                     // Flush output
        usleep(5000);                       // Delay
    }

    printf("\rEncoded Successfully..\n");   // Success message
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)  // Copy 54 byte header
{
    char header[54];                        // Header buffer
    fread(header,54,1,fptr_src_image);      // Read header
    fwrite(header,54,1,fptr_dest_image);    // Write header
    return e_success;                       // Return success
}

Status encode_byte_to_lsb(char data, char *image_buffer)  // Encode 1 byte
{
    for(int i = 0; i < 8; i++)              // Loop 8 bits
    {
        image_buffer[i] = (image_buffer[i] & (~1));  // Clear LSB
        char get = (data >> i) & 1;         // Extract bit
        image_buffer[i] = image_buffer[i] | get;  // Set LSB
    }
    return e_success;                       // Return success
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)  // Encode integer
{
    char arr[32];                           // 32 byte buffer
    fread(arr, 32, 1, encInfo -> fptr_src_image);  // Read 32 bytes
    encode_integer_to_lsb(file_size, arr);  // Encode integer
    fwrite(arr, 32, 1, encInfo -> fptr_stego_image);  // Write back
    return e_success;                       // Return success
}

Status encode_integer_to_lsb(long file_size, char *arr)  // Encode 32-bit integer
{
    for(int i = 0; i < 32; i++)             // Loop 32 bits
    {
        arr[i] = arr[i] & (~1);             // Clear LSB
        int get = (file_size >> i) & 1;     // Extract bit
        arr[i] = arr[i] | get;              // Set LSB
    }
    return e_success;                       // Return success
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)  // Encode extension
{
    int size = strlen(file_extn);           // Get extension length
    char arr[8];                            // Buffer

    for(int i = 0; i < size; i++)           // Loop each char
    {
        fread(arr, 8, 1, encInfo -> fptr_src_image);  // Read 8 bytes
        encode_byte_to_lsb(file_extn[i], arr);        // Encode char
        fwrite(arr, 8, 1, encInfo -> fptr_stego_image);  // Write back
    }

    return e_success;                       // Return success
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)  // Copy remaining bytes
{
    char ch;                                // Temporary variable

    while((fread(&ch,1,1,fptr_src)) == 1)   // Read until EOF
    {
        fwrite(&ch,1,1,fptr_dest);          // Write byte
    }

    return e_success;                       // Return success
}