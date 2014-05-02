#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


using namespace std;


int main()
{
	int x=0;
	int y=10;
	char ccc[100];
	strcpy(ccc,"aaaa");
	for(int i=0;i<20;i++)
	{
		cout<<i<<endl;
		char a[5];sprintf(a,"%d",i);string s(a);string ss[3];
	//	(s.find_first_of("3")!=string::npos)?cout<<"hello"<<endl:(i%3==0)?cout<<"boy"<<endl:cout<<i<<endl;
	   (i%3==0)?strcpy(ccc,"aaaa"):strcpy(ccc,"bbbb");
	   (ccc)?cout<<1<<endl:cout<<2<<endl;

	}
}
