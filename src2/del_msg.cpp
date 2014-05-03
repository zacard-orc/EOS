#include "tools.h"

using namespace std;

int main()
{
	string line;
	while(getline(cin,line))
	{
		 msgctl(atoi(line.c_str()),IPC_RMID,(struct msqid_ds *)0);		
		 cout<<"DEL MQ OK:"<<line<<endl;
	}
}
