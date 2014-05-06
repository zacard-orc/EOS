#include "tools.h"

#define  SERVER_PORT 9999
#define  CLIENT_PORT ((20002+rand())%65536) 
#define  BUFFER_SIZE 100
#define  SERVER_IP  "192.168.137.105"

int clifd; //定义客户端socket FD
int t_timer=0;


void *DmTimer(void *arg)
{
   while(1)
   {
	  t_timer++;
      if(t_timer>5) 
		close(clifd);
	  sleep(1);
   }
   return (void *)0;
}

int main()
{
	//设置去缓存I/O
	std::ios::sync_with_stdio(false);
	//定义客户端socket 参数
	int length=0,flag_reconn=-1;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t socklen = sizeof(servaddr);
	char buf[BUFFER_SIZE];
	if((clifd=socket(AF_INET,SOCK_STREAM,0)) < 0  )
	{
	  printf("create socker error!\n");
	  exit(1);	
	}

	bzero(&cliaddr,sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(0);
	cliaddr.sin_addr.s_addr = htons(INADDR_ANY);	 	
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	inet_aton(SERVER_IP,&servaddr.sin_addr);
	servaddr.sin_port=htons(SERVER_PORT);

	if (bind(clifd,(struct sockaddr*) &cliaddr,sizeof(cliaddr))<0)
	{
		ShowErr("bind error"); 
		exit(1);  
	}
		
    if (connect(clifd,(struct sockaddr *) &servaddr,socklen)<0)  
    {  
        ShowErr("can't connect to server"); 
        exit(1);  
    }   
	
	//临时存储收到的报文
	string line;
	
	//定义ESTABLISH状态下的timer
	pthread_t dm;
    pthread_attr_t attr; 
    pthread_attr_init (&attr);   
    pthread_attr_setscope (&attr, PTHREAD_SCOPE_PROCESS);   
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED); 
    pthread_create(&dm,&attr,DmTimer,NULL);

	while(getline(cin,line))
	{
	  cout<<line<<endl;
	  t_timer=0; //超时计数器归0
      send(clifd,line.c_str(),line.length(),0);
      length=recv(clifd,buf,BUFFER_SIZE,0);
	  if(length<=0)
	  {
	       printf("i'm out\n");
		   sleep(3);
		   //3秒后自动重练
		   while(flag_reconn==-1)
		   {
				sleep(1);
				clifd=socket(AF_INET,SOCK_STREAM,0);
				bzero(&cliaddr,sizeof(cliaddr));
				cliaddr.sin_family = AF_INET;
				cliaddr.sin_port = htons(0);
				cliaddr.sin_addr.s_addr = htons(INADDR_ANY);
				bzero(&servaddr,sizeof(servaddr));
				servaddr.sin_family=AF_INET;
				inet_aton(SERVER_IP,&servaddr.sin_addr);
				servaddr.sin_port=htons(SERVER_PORT);
				cout<<bind(clifd,(struct sockaddr*) &cliaddr,sizeof(cliaddr))<<endl;
				flag_reconn=connect(clifd,(struct sockaddr *) &servaddr,socklen);
				cout<<flag_reconn<<endl;
			}
		   //重练成功
		   flag_reconn=-1;
	  }
	}
    close(clifd);
    
    return 0;  
}
