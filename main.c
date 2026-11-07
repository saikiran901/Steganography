#include <stdio.h>
#include "encode.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encodeInfo;
    DecodeInfo decodeInfo;

    if( argc == 1 )
    {
	// print -> Error + usage msg
	return 0;
    }

    int opr = check_operation(argv[1]);
 
    if( opr == encode )
    {
	if( argc < 4 || argc > 5 )
	{
	    // print -> Error + usage msg
        printf("Error: The Encoding arguments are overbounded\n");
	    return 0;
	}
    if(do_encoding(argv,&encodeInfo)==success)
    {
        printf(".............Encoding completed Successfully.............\n");
    }
    else
    {
        printf("Encoding failed.\n");
    }
}


    else if( opr == decode )
    {
	if( argc < 3 || argc > 5 )	
	{
	    // print -> Error + usage msg
         printf("Error: Invalid number of  Decoding arguments.\n");
	    return 0;
	}


        if(do_decoding(argv,&decodeInfo)==success)
        {
       printf(".............Decoding completed Successfully.............\n");
        }
    else
    {
    printf("Decoding Failed.\n");
    }

    }
    
    else
    {
	// Print -> Error + usage msg
    printf("Invalid arguments\n");
    }
    return 0;
}
