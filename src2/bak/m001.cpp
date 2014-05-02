//mq  创建和删除
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

using namespace std;


int main()
{
	key_t key;
	int mid;

	key=ftok("/root/tt/mq",0x881);
	if((mid = msgget(key, IPC_CREAT|0770)) == -1) {
	       perror("message queue create error");
	       exit(1);
    }
	printf("0x%X\n",mid);
	system("ipcs -q");
	msgctl(mid,IPC_RMID,(struct msqid_ds *)0);
    system("ipcs -q");
    exit(0);
}

