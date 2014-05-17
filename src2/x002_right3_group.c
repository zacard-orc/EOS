//群ping的设置
#include <stdio.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

//宏定义参数
#define TNUM 4
#define PACKET_SIZE 1024
#define MAX_WAIT_TIME 3
#define MAX_NO_PACKETS 5
#define TMOUT 2000

//全局变量与统计、操作符号相关
int nsend[TNUM]={0},nreceived[TNUM]={0},nloss[TNUM]={0};  //发、收、失败
int sockfd[TNUM];
double ndelay[TNUM][MAX_NO_PACKETS];
struct sockaddr_in dest_addr[TNUM];
struct sockaddr_in from_addr[TNUM];


unsigned short cal_chksum(unsigned short *addr,int len);
int pack(int pack_no,char icmp_hd[],int id);
int unpack(char *buf,int len,struct sockaddr_in src_addr,struct timeval *tvrecv,int ndnum,int id);
void send_packet(int sockfd,struct sockaddr_in dest_addr,int id);
void recv_packet(int sockfd,struct sockaddr_in src_addr,int id);
double tv_sub(struct timeval *out,struct timeval *in);

void statistics(int id)
{
//		printf("-------- %d --------\n",id);
		printf("ID=%d,SEND=%d,LOSS=%d,%%%d\n",id,nsend[id],nloss[id],100-(nsend[id]-nloss[id])/nsend[id]*100);
		int i=0;
		double sum=0,max=0;
		for(i=0;i<MAX_NO_PACKETS;i++)
		{
//			printf("%.3f ",ndelay[id][i]);
			sum+=ndelay[id][i];
			if(ndelay[id][i]>max) max=ndelay[id][i];
		}
		printf("ID=%d,AVG=%.2f,MAX=%.2f\n",id,sum/MAX_NO_PACKETS,max);
}


/*校验和算法*/
unsigned short cal_chksum(unsigned short *addr,int len)
{       
		int nleft=len;
        int sum=0;
        unsigned short *w=addr;
        unsigned short answer=0;
                
		/*把ICMP报头二进制数据以2字节为单位累加起来*/
        while(nleft>1)
        {       sum+=*w++;
                nleft-=2;
        }
		//若ICMP报头为奇数个字节，会剩下最后一字节。把最后一个字节视为一个2字节数据的高字节，这个2字节数据的低字节为0，继续累加
        if( nleft==1)
        {       *(unsigned char *)(&answer)=*(unsigned char *)w;
                sum+=answer;
        }
        sum=(sum>>16)+(sum&0xffff);
        sum+=(sum>>16);
        answer=~sum;
        return answer;
}


/*设置ICMP报头*/
int pack(int pack_no,char icmp_hd[],int id)
{       
		int i,packsize;
        struct icmp *icmp;
        struct timeval *tval;
        icmp=(struct icmp*)icmp_hd;
        icmp->icmp_type=ICMP_ECHO;
        icmp->icmp_code=0;
        icmp->icmp_cksum=0;
        icmp->icmp_seq=pack_no;
        icmp->icmp_id=id;
        packsize=200;              //发送的报文长度
        tval=(struct timeval *)icmp->icmp_data;
        gettimeofday(tval,NULL);    //记录发送时间
        icmp->icmp_cksum=cal_chksum((unsigned short *)icmp,packsize); //校验算法
        return packsize;
}


/*发送ICMP报文*/
void send_packet(int sockfd,struct sockaddr_in dest_addr_f,int id)
{       
	    //printf("%s\n",inet_ntoa(dest_addr_f.sin_addr));
	    char sendpacket[PACKET_SIZE];  //icmp发送包头
		int packetsize;
		nsend[id]++;
		//printf("%d\n",nsend);
        packetsize=pack(nsend[id],sendpacket,id); /*设置ICMP报头*/
        //printf("%d\n",sendto(sockfd,sendpacket,packetsize,0,(struct sockaddr *)&dest_addr,sizeof(dest_addr))) ;
		sendto(sockfd,sendpacket,packetsize,0,(struct sockaddr *)&dest_addr_f,sizeof(dest_addr_f));

}

/*接收所有ICMP报文*/
void recv_packet(int sockfd,struct sockaddr_in src_addr_f,int id)
{       
	    //printf("sssssssssss\n");
		//printf("%s\n",inet_ntoa(src_addr_f.sin_addr));
		int n,fromlen;
        extern int errno;
		char recvpacket[PACKET_SIZE];  //icmp接受包头
        //signal(SIGALRM,statistics);
        fromlen=sizeof(src_addr_f);
		struct timeval tvrecv;
        while(nreceived[id]<nsend[id])
        {       
			   //printf("%d,%d\n",nreceived[id],nsend[id]);
				if(recvfrom(sockfd,recvpacket,sizeof(recvpacket),0,(struct sockaddr *)&src_addr_f,&fromlen)==-1)
				{
					printf("%d : %s time-out %dms,seq=%d\n",id,inet_ntoa(src_addr_f.sin_addr),TMOUT,nloss[id]);				
					ndelay[id][nreceived[id]]=TMOUT;
					nloss[id]++;
					nreceived[id]++;
					
				} else
				{
                gettimeofday(&tvrecv,NULL);  /*记录接收时间*/
				//printf("zzzzzzzzzz\n");
                if(unpack(recvpacket,n,src_addr_f,&tvrecv,nreceived[id],id)==-1)continue;
                nreceived[id]++;
				}

        }
}


/*剥去ICMP报头*/
int unpack(char *buf,int len,struct sockaddr_in src_addr_f,struct timeval *tvrecv,int ndnum,int id)
{       
	   // printf("zzzzzz\n");
		int i,iphdrlen;
        struct ip *ip;
        struct icmp *icmp;
        struct timeval *tvsend;
        double  rtt;
        ip=(struct ip *)buf;
        iphdrlen=ip->ip_hl<<2;    /*求ip报头长度,即ip报头的长度标志乘4*/
        icmp=(struct icmp *)(buf+iphdrlen);  /*越过ip报头,指向ICMP报头*/
        len-=iphdrlen;            /*ICMP报头及ICMP数据报的总长度*/
        if(len<8)                /*小于ICMP报头长度则不合理*/
        {       
			printf("ICMP packets/'s length is less than 8\n");
            return -1;
        }
	    if((icmp->icmp_type==ICMP_ECHOREPLY) && (icmp->icmp_id==id))
        {       
				tvsend=(struct timeval *)icmp->icmp_data;
				rtt=ndelay[id][ndnum]=tv_sub(tvrecv,tvsend);
                printf("%d : %d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n",id,
                        len,
                        inet_ntoa(src_addr_f.sin_addr),
                        icmp->icmp_seq,
                        ip->ip_ttl,
                        rtt);
        }
        else return -1;
}


double tv_sub(struct timeval *out,struct timeval *in)
{
	double delay_u,delay_s;
	if((out->tv_usec-in->tv_usec)<0)
    {  
	    delay_u=(double)(out->tv_usec+1000000-in->tv_usec)/1000;
		delay_s=(double)(out->tv_sec-in->tv_sec-1);

	}
	else
	{
		delay_u=(double)(out->tv_usec-in->tv_usec)/1000;	
		delay_s=(double)(out->tv_sec-in->tv_sec);
	}
	return delay_s*1000+delay_u;
}


void *worker(void *arg)
{
	int p_id=*(int *)arg;
	int i=0;
	//pthread_t tid=pthread_self();
	//printf("%u\n",tid);
	//printf("%s\n",inet_ntoa(from_addr[p_id].sin_addr));
	//printf("%s\n",inet_ntoa(dest_addr[p_id].sin_addr));
	for(i=0;i<MAX_NO_PACKETS;i++)	
	{
		send_packet(sockfd[p_id],dest_addr[p_id],p_id); 
		recv_packet(sockfd[p_id],from_addr[p_id],p_id);
		usleep(500000);
	}
	statistics(p_id);

}


main(int argc,char *argv[])
{      
		//设置多线程
		pthread_t ntid[TNUM];

		//设置SOCKET初始变量
	    struct hostent *host;
        struct protoent *protocol;
		int i=0;

		//设置超时时间
		struct timeval tv_tmt;
		tv_tmt.tv_sec=TMOUT/1000;
		tv_tmt.tv_usec=0;
		
		
		//批量设置socket的属性
        protocol=getprotobyname("icmp");
		for(i=0;i<TNUM;i++)
		{
			sockfd[i]=socket(AF_INET,SOCK_RAW,protocol->p_proto) ;
			setsockopt(sockfd[i],SOL_SOCKET,SO_RCVTIMEO,&tv_tmt,sizeof(tv_tmt));
			bzero(&dest_addr[i],sizeof(dest_addr[i]));
			dest_addr[i].sin_family=AF_INET;
			inet_aton("127.0.0.1",&dest_addr[i].sin_addr); 
			//inet_aton("99.99.22.133",&dest_addr[i].sin_addr);
		}
		   inet_aton("99.99.22.133",&dest_addr[1].sin_addr);

		for(i=0;i<TNUM;i++)
		{
			pthread_create(&ntid[i],NULL,worker,&i);				
			sleep(1);
			//usleep(500);
		}

  		//inet_aton("99.99.22.133",&dest_addr.sin_addr);

		sleep(15);
        return 0;
}

