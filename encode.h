/*Description  : Header file for Encoding module.
                Contains:
                  1. EncodeInfo structure
                  2. Macro definitions
                 3. Function declarations used in encoding
*/

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
    char *src_image_fname; // .bmp file name
    FILE *fptr_src_image; // file pointer for .bmp file
    uint image_capacity; // here we are storing the image size
    uint bits_per_pixel; 
    char image_data[MAX_IMAGE_BUF_SIZE]; 

    /* Secret File Info */
    char *secret_fname; // .txt bmp
    FILE *fptr_secret;  // file pointer for txt file
    char extn_secret_file[MAX_FILE_SUFFIX]; // here we will be store the extension of the secret file.
    char *secret_data; // here we will be storing the secret text.
    long size_secret_file; // here we will store the size of the secret text
    long size_secret_file_extn; // here we will store the size of the extension.

    /* Stego Image Info */
    char *stego_image_fname; //here we will store the encoded file name
    FILE *fptr_stego_image; //file pointer

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]); // Completed

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo); //completed

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

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

/*Encode a integer into lsb of the .bmp file*/
Status encode_integer_to_lsb(long file_size, char *arr);


#endif
