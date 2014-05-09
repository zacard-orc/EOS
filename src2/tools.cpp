#include "tools.h"

using namespace std;

extern int errno;

void ShowErr(string ermsg)
{
	cout<<"ERROR-PMSG:"<<ermsg<<endl;
	cout<<"ERROR-CODE:"<<errno<<":"<<strerror(errno)<<endl;
};


void SetUnBlock(int sock)
{
   int opts;
   opts=fcntl(sock,F_GETFL);
   if(opts<0)
   {
      ShowErr("fcntl(sock,GETFL)");
      exit(1);
   }
   opts = opts|O_NONBLOCK;
   if(fcntl(sock,F_SETFL,opts)<0)
   {
      ShowErr("fcntl(sock,SETFL,opts)");
      exit(1);
   }
};


int StartMQ(int proj_id)
{
	int mq_id;
    key_t key=ftok("/root/tt/mq",proj_id);
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


void init_daemon(void)
{       
    int pid,i;
    if((pid=fork()))
        exit(0);
    else if(pid<0)
        exit(1);
    setsid();
    if((pid=fork()))
        exit(0);
    else if(pid<0)
        exit(1);
    
    for(i=0;i<NOFILE;++i)
    {   
        close(i);
    }
    chdir("/root/tt/tmp");
    umask(0);
    signal(SIGCHLD,SIG_IGN);
    return;
}



