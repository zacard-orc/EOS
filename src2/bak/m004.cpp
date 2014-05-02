//mq.client
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

#define TEST 0x77
#define MAX_TEST 100

using namespace std;


struct mybuf
{
	int type;
	char msg_text[MAX_TEST];
};


int main()
{
	int mq_id,ret,type;
	key_t key;
	char buf[MAX_TEST];
	struct mybuf data;
	
	key=ftok(".",TEST);
	if(key==-1){
		perror("ftok error");
		exit(1);
	}
	
	mq_id=msgget(key, IPC_CREAT|0777);
	if(mq_id==-1) {perror("msgget error");exit(2);}

	data.type = 0;
	strcpy(data.msg_text,"zczccxc");

	//while(1 && data.type<100000)
	while(1)
	{
		ret = msgsnd(mq_id,(void *)&data,100,0);	
		//cout<<ret<<":"<<data.type<<endl;
		data.type++;
		usleep(1);
	}
	//cout<<data.type<<endl;
	



}
