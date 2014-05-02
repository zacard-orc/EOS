#include <stdio.h>  
#include <sys/socket.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <netinet/in.h>  
#include <stdlib.h>   
#include <string.h>
#include <time.h>
#include <fcntl.h>

#define  SERVER_PORT 8888
#define  CLIENT_PORT ((20002+rand())%65536) 
#define  BUFFER_SIZE 255 
#define  REUQEST_MESSAGE "welcome to my server.\n"   
#define  SERVER_IP  "192.168.137.105"



int comdelay(struct timeval a,struct timeval b)
{
	int c;
	c=(b.tv_sec-a.tv_sec)*1000+(b.tv_usec-a.tv_usec)/1000;
	return c;
}


int main()
{
	int servfd,clifd,length=0;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t socklen = sizeof(servaddr);
	char buf[BUFFER_SIZE];\
	struct timeval tvafter,tvpre;
	

	if((clifd=socket(AF_INET,SOCK_STREAM,0)) < 0  )
		 {
		  printf("create socker error!\n");
		  exit(1);	
		}
		
	srand(time(NULL));
	
	bzero(&cliaddr,sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(0);
	cliaddr.sin_addr.s_addr = htons(INADDR_ANY);	 	
	
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	inet_aton(SERVER_IP,&servaddr.sin_addr);
	servaddr.sin_port=htons(SERVER_PORT);
	
	gettimeofday(&tvpre,NULL);	
	
	if (bind(clifd,(struct sockaddr*) &cliaddr,sizeof(cliaddr)) <0)
	{
		printf( "bind to port %d failure!\n",CLIENT_PORT);  
		exit( 1 );  
	}
		
    if (connect(clifd,(struct sockaddr *) &servaddr,socklen)  <   0 )  
    {  
        printf( " can't connect to %s!\n ", SERVER_IP);  
        exit( 1 );  
    }   
    //=========================================================
    //int recv(int s, void *buf, size_t len, int flags)
    //=========================================================   
       
    printf("ssssssssssss\n"); 
    //length=recv(clifd, buf, BUFFER_SIZE, 0);
    	 /*
    if (length < 0)  
    {  
        printf( " error comes when recieve data from server %s! ", SERVER_IP );  
        exit( 1 );  
    } 
	*/
    
  
	int i;
	char x[]="10.11.1.2.3 Hello Girl!\n";
	char y[]="woodoo\n";
	printf("%d\n",sizeof(x));
	for(i=0;i<1000000;i++)
	{
      send(clifd,x,24,0);
      length=recv(clifd, buf, BUFFER_SIZE, 0);
	}
//	gettimeofday(&tvafter,NULL);
//	printf("recdata:%s:%s:%d\n",SERVER_IP,buf,comdelay(tvpre,tvafter));
 
//	sleep(10);
//	send(clifd,y,7,0);
//	send(clifd,y,7,0);


    close(clifd);
    
    return 0;  
}
