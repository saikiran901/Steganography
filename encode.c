#include <stdio.h>
#include "encode.h"
//#include "types.h"
#include<string.h>


/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
unsigned int get_image_size_for_bmp(FILE *fptr_image)
{
    unsigned int width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: success or failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    encInfo->src_image_fptr = fopen(encInfo->src_image_fname, "r");

    if (encInfo->src_image_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return failure;
    }

    encInfo->secret_fptr = fopen(encInfo->secret_fname, "r");

    if (encInfo->secret_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return failure;
    }

    encInfo->output_image_fptr = fopen(encInfo->output_image_fname, "w");

    if (encInfo->output_image_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->output_image_fname);

    	return failure;
    }

    return success;
}


/* Check operation type */
Opr_type check_operation(char *option)
{
if(strcmp(option, "-e")==0)
{
    return encode;
}
else if(strcmp(option, "-d")==0)
{
    return decode;
}
else
{
    return unsupported;
}
}



/* Read and validate Encode args from argv */
Status validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
 int len;
 len = strlen(argv[2]); 
 if (len >= 4 && strcmp(argv[2] + len - 4, ".bmp")==0)
 {
    encInfo->src_image_fname = argv[2];
 }
 else
 {
    printf("Error: Source image must be a .bmp file\n");
    return failure;
 }

//  len = strlen(argv[3]);
 
//     if(len >=4 && strcmp(argv[3] + len -4, ".txt")==0)
//  {
//     encInfo->secret_fname = argv[3];
//     strcpy(encInfo->secret_extn, ".txt");
//     encInfo->secret_extn_size = strlen(".txt");

//  }
//  else
//  {
//     printf("Error: Secret file must have .txt extension\n");
//     return failure;
//  }


char *extn = strrchr(argv[3], '.');

if (extn != NULL &&
    (strcmp(extn, ".txt") == 0 ||
     strcmp(extn, ".c") == 0 ||
     strcmp(extn, ".cpp") == 0 ||
     strcmp(extn, ".py") == 0 ||
     strcmp(extn, ".sh") == 0))
{
    encInfo->secret_fname = argv[3];

    strcpy(encInfo->secret_extn, extn);
    encInfo->secret_extn_size = strlen(extn);
}
else
{
    printf("Error: The extension is not supported\n");
    return failure;
}

  //3. check the o/p  image file name
  if(argv[4]==NULL)
  {
    encInfo->output_image_fname = "stego.bmp";
  }
    else
{
  len = strlen(argv[4]);
  if(len >=4 && strcmp(argv[4] + len - 4, ".bmp")==0)
  {
    encInfo->output_image_fname = argv[4];
  }
  else
  {
    printf("Error: Ouput image must have .bmp extension\n");
    return failure;
  }
}
return success;
}


/* Perform the encoding */
Status do_encoding(char *argv[], EncodeInfo *encInfo)
{
    if(validate_encode_args(argv,encInfo)== failure)
    {
        printf("Validation failed.\n");
        return failure;
    }

    if(open_files(encInfo)==failure)
    {
        printf("File opened failed.\n");
        return failure;
    }

    encInfo->secret_file_size = get_file_size(encInfo->secret_fptr);

    if(check_capacity(encInfo)==failure)
    {
       printf("Insufficient image capacity.\n"); 
        return failure;
    }

    if(copy_bmp_header(encInfo)==failure)
    {
        printf("bmp header  copying failed.\n");
        return failure;
    }

    if(encode_magic_string(MAGIC_STRING,encInfo)==failure)
    {
        printf("Magic string encoded failed.\n");
        return failure;
    }

    if(encode_secret_file_extn(encInfo)==failure)
    {
        return failure;
    }

    if(encode_secret_file_data(encInfo)==failure)
    {
        return failure;
    }

    if(copy_remaining_img_data(encInfo)==failure)
    {
        return failure;
    }
    printf("Encoding completed successfully\n");
 return success;
}


/* check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
unsigned int image_capacity;
unsigned int required_size;
image_capacity= get_image_size_for_bmp(encInfo->src_image_fptr);

required_size=(encInfo->secret_file_size + strlen(MAGIC_STRING) + encInfo-> secret_extn_size + 4) * 8;
if(image_capacity >= required_size)
{
    return success;
}
else
{
    return failure;
}
}


/* Get secret file size */
unsigned int get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    unsigned int size = ftell(fptr);
    rewind(fptr); // Move file pointer back to beginning
    return size;

}


/* Copy bmp image header */
Status copy_bmp_header(EncodeInfo *encInfo)
{
    char buffer[54];
    rewind(encInfo->src_image_fptr);

    if(fread(buffer, 1, 54, encInfo->src_image_fptr)!=54)
    {
        return failure;
    }
    if(fwrite(buffer, 1, 54, encInfo->output_image_fptr)!=54)
    {
        return failure;
    }
    return success;
}


/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int i;
    char img_bytes[8];
  
for(i=0; magic_string[i]!='\0'; i++)
{
    if(fread(img_bytes, 1, 8, encInfo->src_image_fptr)!=8)
    {
        return failure;
    }
     
           if(encode_1byte_to_lsb(magic_string[i], img_bytes)== failure)
           {
            return failure;
           } 

        if(fwrite(img_bytes, 1, 8, encInfo->output_image_fptr)!=8)
        {
            return failure;
        }

    }

       return success;
}


/* Encode secret file extenstion */
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    char img_buffer[32];
    int i;
    //encode extn size
    if(fread(img_buffer, 1 , 32, encInfo->src_image_fptr)!=32)
    {
        return failure;
    }
    if(encode_4byte_to_lsb(encInfo->secret_extn_size, img_buffer)==failure)
    {
        return failure;
    }
    if(fwrite(img_buffer, 1 , 32, encInfo->output_image_fptr)!=32)
    {
        return failure;
    }
// encode the extn

for(i=0; i<encInfo->secret_extn_size; i++)
{
    char buffer[8];
    if(fread(buffer, 1, 8, encInfo->src_image_fptr)!=8)
    {
        return failure;
    }
    if(encode_1byte_to_lsb(encInfo->secret_extn[i], buffer)== failure)
    {
        return failure;
    }
    if(fwrite(buffer, 1, 8, encInfo-> output_image_fptr)!=8)
    {
        return failure;
    }
}
return success;
}




/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("Strated with secret file data.\n");
    char buffer[8];
    char secret_byte;
    
    char img_bytes[32];
    if(fread(img_bytes, 1, 32, encInfo->src_image_fptr)!=32)
    {
        return failure;
    }
    if(encode_4byte_to_lsb(encInfo->secret_file_size, img_bytes)==failure)
    {
        return failure;
    }
     if(fwrite(img_bytes, 1, 32, encInfo->output_image_fptr)!=32)
    {
        return failure;
    }

    for(int i=0; i<encInfo->secret_file_size; i++)
    {
        fread(&secret_byte, 1, 1,encInfo->secret_fptr);
      // printf("Original byte: %02X\n", (unsigned char)secret_byte);
        fread(buffer, 1, 8,encInfo->src_image_fptr);
        encode_1byte_to_lsb(secret_byte, buffer);
      // printf("Encoded byte: %02X\n", (unsigned char)secret_byte);
        fwrite(buffer, 1, 8,encInfo->output_image_fptr);

        
    }
    printf("Completed Succesfully.\n");
    return success;
}




/* Encode 1 byte into LSB of image data array */
Status encode_1byte_to_lsb(char data, char *buffer_8)
{
    int i;
    for(i=7; i>=0; i--)
    {
        int bit=(data &(1<<i));
        bit=bit >>i;
        int mask = (1<<0);
        buffer_8[7-i]=buffer_8[7-i]&(~mask);
        buffer_8[7-i]=buffer_8[7-i] | bit;
    }
    return success;

}

/* Encode 4byte(int) into LSB of image data array */
Status encode_4byte_to_lsb(int data, char *buffer_32)
{
    int i;
    for(i=31; i>=0; i--)
    {
        int bit=(data &(1<<i));
        bit=bit >> i;
        int mask = (1<<0);
        buffer_32[31-i]=buffer_32[31-i]&(~mask);
        buffer_32[31-i]=buffer_32[31-i] | bit;
    }
    return success;
}



/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(EncodeInfo * encInfo)
{
    printf("Started copy the Remaining Data. \n");
    int ch;
   while ((ch = fgetc(encInfo->src_image_fptr)) != EOF)
    {
        fputc(ch,encInfo->output_image_fptr);
    }
    printf("Encoding done Successfully.\n");
    return success;
}

//#endif
