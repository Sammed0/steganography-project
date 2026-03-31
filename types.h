/*Description  : Contains user-defined data types used in the project.
                  Defines:
                  1. uint  → Unsigned integer alias
                  2. Status → Function return status (success/failure)
                  3. OperationType → Type of operation (encode/decode)
 */

#ifndef TYPES_H            // Prevent multiple inclusion of this header file
#define TYPES_H            // Define macro TYPES_H

/* User defined types */
typedef unsigned int uint;  // Define uint as alias for unsigned int

/* Status will be used in function return type */
typedef enum
{
    e_success,              // Represents successful execution
    e_failure               // Represents failure in execution
} Status;                   // Enum name is Status

/* OperationType defines type of operation */
typedef enum
{
    e_encode,               // Encode operation selected
    e_decode,               // Decode operation selected
    e_unsupported           // Invalid or unsupported operation
} OperationType;            // Enum name is OperationType

#endif                      // End of header guard