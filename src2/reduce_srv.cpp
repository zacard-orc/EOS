/* ����:Zacard
*  EPOLL ������ ������ȡ�����ⲿ����Ϣ����ת����������Ϣ����
*/
#include "tools.h"

#define MAXCHARNUM 512         //��SOCKET��ȡ���ַ���
#define LISTENQ 2000        //�������г���
#define SERV_PORT 9999      //�����ṩ�ķ���˿�
#define EP_INS_NUM 2048     //�ܹ���ض��ٸ�EPOLLʵ����
#define EP_HINS_NUM 2000	//ÿһ��WAITʱ�ܴ����EPOLLʵ����
#define EP_TXT_LEN 512     //ÿ���յ��ı��ĳ����趨
#define HELLO  "ICU"

using namespace std;

int nfds=0;


//��ش����EPOLLʵʱ�¼���
void *dmlog(void *arg)
{
   while(1)
   {
	  cout<<"ONLINE PERS:";
      (nfds<=1)?cout<<1<<endl:cout<<nfds<<endl;
	  usleep((long)arg*1000);
   }
   return (void *)0;
}


int main()
{
	 //���������SOCKET����
     struct sockaddr_in clientaddr,serveraddr;
	 int listenfd,connfd,sockfd,epfd;  
	 int n_call=0;    //�����õĴ���
     int n_client_len;   //�ͻ��˵ĵ�ַ����
     int n_readflag;     //�Ӷ˿��ж����ݵĳɹ�ʧ�ܱ�ʾ
	 listenfd=socket(AF_INET,SOCK_STREAM,0);
	 SetUnBlock(listenfd);

	 
     bzero(&serveraddr, sizeof(serveraddr));
     serveraddr.sin_family = AF_INET;  //�趨Э���
     serveraddr.sin_addr.s_addr =htons(INADDR_ANY);  //������ַ
     serveraddr.sin_port=htons(SERV_PORT); //�˿ں�
	 
     bind(listenfd,(struct sockaddr *)&serveraddr, sizeof(serveraddr));
     listen(listenfd,LISTENQ);

     
     //���������û���ͳ�Ƶ��߳�
     pthread_t dm;
     pthread_attr_t attr; 
     pthread_attr_init (&attr);   
     pthread_attr_setscope (&attr, PTHREAD_SCOPE_PROCESS);   
     pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED); 
     pthread_create(&dm,&attr,dmlog,(void *)10000);

     
     //����EPOLL�¼�������Ϣ
	 struct epoll_event ev,events[EP_HINS_NUM];
	 char RECV_MSG[EP_HINS_NUM][EP_TXT_LEN];
	 char ANSW_MSG[EP_TXT_LEN]=HELLO;
	 epfd=epoll_create(EP_INS_NUM);  //ע����ʵ����
     ev.data.fd=listenfd;
     ev.events=EPOLLIN|EPOLLET;
     epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev); //ע��������



	 while(1)
	 {
          nfds=epoll_wait(epfd,events,EP_HINS_NUM,-1); //��ѵ������

          for(int i=0;i<nfds;++i)
          {
               if(events[i].data.fd==listenfd)    
               {
					//cout<<"Step 1 start"<<endl;
					n_client_len = sizeof(clientaddr); 
                    connfd = accept(listenfd,(struct sockaddr *)&clientaddr, (socklen_t *)&n_client_len);
                    if(connfd<0){
                         perror("connfd<0");
                         exit(1);
                    }
					cout<<"RECV..."<<endl;
					SetUnBlock(connfd);
                    ev.data.fd=connfd;
                    ev.events=EPOLLIN|EPOLLET; 
                    epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
					//cout<<"Step 1 end"<<endl;
               }
			   //������Ѿ����ӵ��û��������յ����ݣ���ô���ж��롣
			   //������������¶���EPOLL EVENT ��ע����FD�����û���ݣ���ر�EPOLL.EVENTS.DATA.FD
               else if(events[i].events&EPOLLIN)    
               {
				    //cout<<"Step 2 start"<<endl;
                    if ((sockfd = events[i].data.fd) < 0) continue;
                    if ((n_readflag= read(sockfd, RECV_MSG[i], MAXCHARNUM)) < 0) {
                         if (errno == ECONNRESET) {
                              close(sockfd);
                              events[i].data.fd = -1;
                         } else
	    				cout<<"readmsg error\n"<<endl;
                    } else if (n_readflag == 0) {
                        events[i].data.fd = -1;
                    } else {
						cout<<RECV_MSG[i]<<endl;
						//cout<<n_call++<<"__"<<RECV_MSG[i]<<endl;
					}
					ev.data.fd=sockfd;
                    ev.events=EPOLLOUT|EPOLLET;
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
					//cout<<"Step 2 end"<<endl;
               }
               else if(events[i].events&EPOLLOUT)
               {   
			   		//cout<<"Step 3 start"<<endl;
                    sockfd = events[i].data.fd;
                   	write(events[i].data.fd,ANSW_MSG,strlen(ANSW_MSG));
                    bzero(RECV_MSG[i],EP_TXT_LEN);
                    ev.data.fd=sockfd;
                    ev.events=EPOLLIN|EPOLLET;
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
					//cout<<"Step 3 end"<<endl;
               }
          }
     }
}



