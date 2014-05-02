#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <errno.h>
#include <string.h>

extern int errno;


using namespace  std;

int main()
{
	const char* input_filename = "not_found.txt";
	FILE* input = fopen( input_filename, "r" );
	if(input==NULL ) {
		 char error_msg[255];
		 sprintf( error_msg, "Error opening file '%s'", input_filename );
		 cout<<"ERROR-CODE:"<<errno<<":"<<strerror(errno)<<endl;
		 //perror(error_msg);
		 exit(-1);
	 }
}
