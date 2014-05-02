//mq.server
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

using namespace std;

#define TEST 0x77
#define MAX_TEXT 100

struct mybuf{
	int type;
	char msg_text[MAX_TEXT];
};


int main()
{
	int mq_id,ret;
	key_t key;
	char buf[MAX_TEXT];
	struct mybuf data;
	key=ftok(".",TEST);

	if(key==-1)
	{
		perror("ftok error");
		exit(1);
	}

	mq_id=msgget(key,IPC_CREAT|0777);
	if(mq_id==-1){
		perror("msgget error");
		exit(2);
	}
	system("date");
	while(1)
	{
		ret=msgrcv(mq_id, (void *)&data,MAX_TEXT,0,0);
	
		/*
		if(strncmp(data.msg_text, "exit", 4) == 0) {
		   break;
		}
		*/
		
		if(ret==-1)
		{
			printf("%d\n",ret);
			exit(3);	
			perror("msgrcv error");
		}
		cout<<data.type<<":"<<data.msg_text<<endl;
		//msgctl(mq_id, IPC_RMID, (struct msqid_ds *)0);
	}
	system("date");
	return 0;
}

