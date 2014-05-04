#include "tools.h"

#define  SERVER_PORT 9999
#define  CLIENT_PORT ((20002+rand())%65536) 
#define  BUFFER_SIZE 100
#define  SERVER_IP  "192.168.137.105"

int main()
{
	std::ios::sync_with_stdio(false);
	int clifd,length=0;
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

	string line;//收到的报文

	while(getline(cin,line))
	{
	  cout<<line<<endl;
      send(clifd,line.c_str(),line.length(),0);
      length=recv(clifd,buf,BUFFER_SIZE,0);
	}
    close(clifd);
    
    return 0;  
}
