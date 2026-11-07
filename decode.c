#include<stdio.h>
#include "decode.h"
#include<string.h>


Status validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // check image stego extension.
    int len;
    len = strlen(argv[2]);
    if(len >=4 && strcmp(argv[2] + len - 4, ".bmp")==0)
    {
        strcpy(decInfo->stego_name, argv[2]);
    }
    else
    {
        printf("Error-> Stego image must be a .bmp file\n");
        return failure;
    }
 // set output secret file name.
 if(argv[3]!=NULL)
 {
    strcpy(decInfo->output_secret_fname, argv[3]);
 }
 else
 {
    strcpy(decInfo->output_secret_fname, "output");
 }
 return success;
}

Status do_decoding(char *argv[], DecodeInfo *decInfo)
{
    if(validate_decode_args(argv,decInfo)== failure)
    {
        return failure;
    }
    if(open_img_file (decInfo)== failure)
    {
        return failure;
    }

    if(skip_bmp_header(decInfo) == failure)
{
    fclose(decInfo->stego_img);
    return failure;
}
    fseek(decInfo->stego_img , 54, SEEK_SET);
    if(decode_magic_string(MAGIC_STRING, decInfo)== failure)
    {
        fclose(decInfo->stego_img);
        return failure;
    }
    printf("Magic string decoded Successfully.\n");
    
    if (decode_secret_file_extn(decInfo) == failure) 
    { 
        fclose(decInfo->stego_img);
         return failure;
         }
          if (decode_secret_file_data(decInfo) == failure)
           { 
            fclose(decInfo->secret_fname);
             fclose(decInfo->stego_img); 
             return failure;
             } 
             fclose(decInfo->secret_fname);
              fclose(decInfo->stego_img);
               printf("Decoding completed successfully.\n");
    return success;

}


Status open_img_file(DecodeInfo *decInfo)
{
    decInfo->stego_img= fopen(decInfo->stego_name, "rb");
    if(decInfo->stego_img == NULL)
    {
        printf("Error-> Unable to open the stego image.\n");
        return failure;
    }
    return success;
}


Status skip_bmp_header(DecodeInfo *decInfo)
{
   if(fseek(decInfo->stego_img,54, SEEK_SET)!=0)
   {
    printf("Error->Unable to skip bmp header.\n");
    return failure;
   } 
   return success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char decoded[sizeof(MAGIC_STRING)];
    char buffer[8];
    for(int i=0; i<strlen(magic_string); i++)
    {
        if(fread(buffer, 1, 8, decInfo->stego_img)!=8)
        {
            printf("Error: Unable to read image data.\n");
            return failure;
        }
        if(decode_1byte_from_lsb(&decoded[i], buffer)==failure)
        {
            return failure;
        }
    }

    decoded[strlen(magic_string)]='\0';

     printf("Expected magic string: [%s]\n", magic_string);
    printf("Decoded magic string: [%s]\n", decoded);
    
     if(strcmp(decoded, magic_string)==0)
     {
        printf("magic string matched.\n");
        return success;
     }
     printf("Error-> magic string mismatched.\n");
     return failure;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char img_buffer[32];
    if(fread(img_buffer, 1, 32, decInfo->stego_img)!=32)
    {
        return failure;
    }
    if(decode_4byte_from_lsb(&decInfo->secret_extn_size, img_buffer)== failure)
    {
        return failure;
    }

 if(decInfo->secret_extn_size <=0 || decInfo->secret_extn_size >= sizeof(decInfo->secret_extn))
        {
            printf("Error-> Invalid extension size.\n");
            return failure;
        }


    for(int i=0; i<decInfo->secret_extn_size; i++)
    {
        char buffer[8];
        if(fread(buffer, 1, 8, decInfo->stego_img)!= 8)
        {
            return failure;
        }
        if(decode_1byte_from_lsb(&decInfo->secret_extn[i], buffer)== failure)
        {
            return failure;
        }
       
    }

    decInfo->secret_extn[decInfo->secret_extn_size]='\0';
    printf("Decoded extension : %s \n", decInfo->secret_extn);

/* Concatenate extension with output filename */
  printf("Output file : %s\n", decInfo->output_secret_fname);
  strcat(decInfo->output_secret_fname, decInfo->secret_extn);
  

  /* Open secret output file in write mode */
  decInfo->secret_fname = fopen(decInfo->output_secret_fname, "w");
  if(decInfo->secret_fname == NULL)
  {
    printf("Error-> Unable open to create output file.\n");
    return failure;
  }
  return success;
}


Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char img_bytes[32];
    if(fread(img_bytes, 1, 32, decInfo->stego_img)!=32)
    {
        return failure;
    }
    if(decode_4byte_from_lsb(&decInfo->secret_file_size, img_bytes)==failure)
    {
        return failure;
    }
    printf("Secret file size : %d\n", decInfo->secret_file_size);

    /* Decode actual secret file data */
     for(int i=0; i<decInfo->secret_file_size; i++)
     {
        char buffer[8];
        char secret_byte;

        if(fread(buffer, 1, 8, decInfo->stego_img)!=8)
        {
            return failure;
        }
        if(decode_1byte_from_lsb(&secret_byte, buffer)==failure)
        {
            return failure;
        }
        
        if(fwrite(&secret_byte,1,1, decInfo->secret_fname)!=1)
        {
            return failure;
        }
     } 
     printf("Secret file data decoded successfully.\n");
     return success;
}


Status decode_1byte_from_lsb(char *data, char *buffer_8)
{
    *data = 0;
    for(int i=0; i<8; i++)
    {
        *data = (*data << 1) | (buffer_8[i] & 1);
    }
    return success;
}



Status decode_4byte_from_lsb(int *data, char *buffer_32)
{
    *data = 0;
    
        for(int i=0; i<32; i++)
        {
            *data=(*data<<1) | (buffer_32[i] & 1);
        }
    return success;
}
