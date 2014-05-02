#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "tools.h"


using namespace std;

extern int errno;

void ShowErr(string ermsg)
{
	cout<<"ERROR-PMSG:"<<ermsg<<endl;
	cout<<"ERROR-CODE:"<<errno<<":"<<strerror(errno)<<endl;
};

int StartMQ(int proj_id)
{
	int mq_id;
    key_t key=ftok(".",proj_id);
	if(key==-1)
    {
	   ShowErr("Fork Mqueue FTOK ERROR");
	   return -1;
    } else
	{
		mq_id=msgget(key,IPC_CREAT|0777);
		if(mq_id==-1)
		{
            ShowErr("Fork Mqueue GET ERROR");
            return -1;
        } else
		{
			return mq_id;
		}
	}

};

void Setnonblocking(int sock)
{
   int opts;
   opts=fcntl(sock,F_GETFL);
   if(opts<0)
   {
      perror("fcntl(sock,GETFL)");
      exit(1);
   }
   opts = opts|O_NONBLOCK;
   if(fcntl(sock,F_SETFL,opts)<0)
   {
      perror("fcntl(sock,SETFL,opts)");
      exit(1);
   }
};



