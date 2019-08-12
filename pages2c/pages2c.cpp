/*
 ============================================================================
 Name        : pages2c.cpp
 Author      : gecuy
 Version     :
 Copyright   : Free sofware
 Description : 
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "pages2c.h"




int main( int argc, char * argv[] )
{
	FILE * fIn;
	FILE * fOut = stdout;
	char Line[256];
	char * InputFileName;
	
	if ( argc > 1 ){
		InputFileName = argv[1];
	}
	else {
		InputFileName = (char*)"/home/glecuyer/Projects/IrBridge/WebUi/index.html";
	}
	char * pFileName;
	char ArrayName[256];
	char OutputFileName[256];	
	
	if ( argc > 2 ){
		printf( "argv2 = %s\n", argv[2] );
	}
	
	fIn = fopen( InputFileName, "r" );	
	if ( fIn == NULL ){
		fprintf( stderr, "Cannot find %s\n", InputFileName );
		return -1;
	}
	
	pFileName = strrchr( InputFileName, '/' );
	if ( pFileName != NULL ){
		pFileName++;
	} else {
		pFileName = InputFileName;
	}
	strncpy( ArrayName, pFileName, 256 );
	char * ptr = strchr( ArrayName, '.' );
	if ( ptr != NULL ){
		ptr[0] = '_';
	}
	strncpy( OutputFileName, ArrayName, 256 );	
	//printf("ArrayName = %s\n", ArrayName );
	strcat( OutputFileName, ".c");
	printf("OutputFileName = %s\n", OutputFileName );
	
	//snprintf( OutputFileName, 256, "index_html.c" );
	fOut = fopen( OutputFileName, "w" );
	
	fprintf( fOut, "// File %s \n", InputFileName );
	fprintf( fOut, "// Generated file, do not edit \n\n\n" );
	
	fprintf( fOut, "const unsigned char %s[] = \n", ArrayName );
	if (  strstr(ArrayName, "htm") != NULL 
	   || strstr(ArrayName, "css") != NULL 
	   || strstr(ArrayName, "js") != NULL ) {
		while ( fgets( Line, 256, fIn ) ){
			fprintf( fOut, "  \"");
			for ( unsigned i=0 ; i<strlen(Line) ; i++ ){
				char c = Line[i];
				if ( c != '\r' && c != '\n' ){
					if ( c == '\"' ){
						fputs( "\\\"", fOut );
					}
					else {
						fputc( c, fOut );
					}
				}
			}
			fprintf( fOut, "\\n\"\n" );
		}
		fprintf( fOut, ";\n\n" );
		fprintf( fOut, "unsigned %s_size(void){\n  return sizeof(%s)-1;\n}\n", ArrayName, ArrayName );
	} 
	else {
		// Binary files
		int count=0;
		fprintf( fOut, "{" );
		while ( 1 ){
			unsigned char b;
			int n = fread( &b, 1, 1, fIn );
                        if ( n <= 0 )
                                break;

			if ( count%16 == 0 ){
				fprintf( fOut, "\n  ");
			}
			fprintf( fOut, "0x%02X,", (unsigned)b );
			count++;
		}
		fseek( fOut, -1, SEEK_CUR );
		fprintf( fOut, "};\n\n" );
		fprintf( fOut, "unsigned %s_size(void){\n  return sizeof(%s);\n}\n", ArrayName, ArrayName );
	}		
	
	fprintf( fOut, "\n// EOF\n\n" );
		
	fclose(fIn);
	fclose(fOut);
    return EXIT_SUCCESS;
}
