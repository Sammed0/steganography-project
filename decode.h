/*Header file for decoding module.
 *                Contains:
 *                1. DecodeInfo structure
 *                2. Function declarations for decoding process
 */

#ifndef DECODE_H                     // Prevent multiple inclusion
#define DECODE_H                     // Define header guard macro

#include <stdio.h>                   // For FILE operations
#include "types.h"                   // For Status and custom types

/* Structure to hold decoding information */
typedef struct _DecodeInfo
{
    FILE *fptr_stego_bmp;            // File pointer for encoded BMP image
    FILE *fptr_secret_data;          // File pointer for output secret file
    char magic_string[5];            // To store decoded magic string
    char encoded_file_name[20];      // Name of input stego image
    char secret_text_fname[20];      // Name of output secret file
    int extn_file_size;              // Size of file extension
    char decoded_file_extn[10];      // Store decoded file extension
    int secret_data_size;            // Size of secret data
    char decoded_secret_data[200];   // Store decoded secret content

} DecodeInfo;                       // Structure name

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);   // Validate CLI arguments

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);   // Main decoding function

/* Get File pointer for stego image */
Status open_decode_files(DecodeInfo *decInfo);   // Open input BMP file

/* Skip bmp image header */
Status skip_bmp_header(FILE  *fptr_stego_bmp);   // Skip first 54 bytes

/* Decode one character from LSB */
char decode_data_from_lsb(char *arr);   // Convert 8 LSB bits into character

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);   // Extract and verify magic string

/* Decode extension size */
Status decode_extn_size(DecodeInfo *decInfo);   // Get extension length

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);   // Extract extension

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);   // Extract secret file size

/* Open output secret file */
Status open_secret_file(DecodeInfo *decInfo);   // Create output file

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);   // Extract secret content

/* Decode integer from LSB */
int decode_integer_to_lsb(char *arr);   // Convert 32 LSB bits into integer

/* Write decoded data into secret file */
Status write_decoded_data_to_secretfile(DecodeInfo *decInfo);   // Write output

#endif                                  // End of header guard