#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include "bizhash.h"

#define HashTbSize 0x156AF9   //0x156AF9==1,400,000条数据


HashTb *RO_HPP,*WR_HPP;
unsigned long crytable[0x500];


void ConnMemHashTable(const char *SMemPath,int SMemFd)
//SMemPath     共享内存库的映射路径
//SMemFd       共享内存库的描述符号
{
   	 unsigned long i=0;
	   int shm_id;
     key_t key;
     key = ftok(SMemPath,SMemFd);

     shm_id=shmget(key,0,0);  
     if(shm_id==-1)
     {
      perror("shm_raw_get error");
      exit(0);
     }
     
     RO_HPP=(HashTb*)shmat(shm_id,NULL,0); 
     printf("Initial Address:%X\n",RO_HPP);
     printf("Connecting Share Hash Memory Rawdata.OK!\n");
     printf("HEAD RECORD:%X\n",(*(RO_HPP+0x75376)).Hash1);
	
}

void CrtMemHashTable(char *SMemPath,int SMemFd,char *RawFileName,unsigned long RawMemSize)
//SMemPath     共享内存库的映射路径
//SMemFd       共享内存库的描述符号
//RawFileName  原始文件，分号分割
//RawMemSize   原始内存空间

{
     FILE *fp=NULL;
	   char *tmpstr=NULL;
	   char tmpraw[30];
	   char tmpcallno[12];
	   unsigned long i;
	   unsigned long ulHashValue;
	   unsigned long ulTbIndex;
	 	  // printf("%X   ",ulHashValue);

	   int shm_id;
     key_t key;
     key = ftok(SMemPath,SMemFd);
     
     //先将之前的SHARED MEMOERY 删除
     shm_id=shmget(key,0,0); 
     shmctl(shm_id,IPC_RMID,NULL);
     printf("share merory ind area %d is delete!\n",shm_id);
     
     if(key==-1)
       {
         perror("ftok error");
         exit(0);
       }
     printf("IPC key=%d\n",key);
     //创建新的SHARED MEMOERY
     shm_id=shmget(key,RawMemSize,IPC_CREAT|IPC_EXCL|0600);   //A00000=10MB; 1F400000=500MB；
     if(shm_id==-1)
     {
      perror("shm_raw_get error");
      exit(0);
     }
     
     printf("shm_raw_id=%d\n",shm_id);
     
     
     
     WR_HPP=(HashTb*)shmat(shm_id,NULL,0); 

     printf("ININTAL ADDRESS:%X\n",WR_HPP);
	   
	   tmpstr=(char *)malloc(sizeof(char)*50);
	   fp=fopen(RawFileName,"r");
	   printf("\nStart Building Share Memory Rawdata\n");
	  // printf("%X   ",ulHashValue);
	   while(fgets(tmpstr,60,fp)!=NULL)
	   {
	   				 int a;
	   				 //printf("11111:%s:%s\n",tmpcallno,tmpstr);
             sscanf(tmpstr,"%10s",tmpcallno); 
             sscanf(tmpstr,"%[^\n]",tmpraw);
             //printf("%s:---",tmpraw);
             //printf("22222:%d\n",a);
             i=(HashString(tmpcallno,0)%HashTbSize);
             
             ulHashValue=HashString(tmpcallno,1);
     

             (*(WR_HPP+i)).Hash1=ulHashValue;
             //printf("%X-%X-",i,(*(WR_HPP+i)).Hash1);  //Right
						 //printf("%s-%X-%X-",tmpcallno,i,(*(WR_HPP+i)).Hash1);									
              
             ulHashValue=HashString(tmpcallno,2);
             (*(WR_HPP+i)).Hash2=ulHashValue;
						 (*(WR_HPP+i)).bExist=0;
             //memcpy((*(WR_HPP+i)).Raw_obj,&tmpcallno,12);
             //memcpy((*(WR_HPP+i)).Raw_str,&tmpstr,sizeof(tmpstr));
             strcpy((WR_HPP+i)->Raw_obj,tmpcallno);
             strcpy((WR_HPP+i)->Raw_str,tmpraw);
             //memcpy((WR_HPP+i)->Raw_str,tmpstr,15);
             //printf("%X-%d-%s-",i,(*(WR_HPP+i)).bExist,(WR_HPP+i)->Raw_str); Right
     				 printf("%s-%X-%X-%X-%d-%s-%s\n",tmpcallno,i,(*(WR_HPP+i)).Hash1,(*(WR_HPP+i)).Hash2,(*(WR_HPP+i)).bExist,(WR_HPP+i)->Raw_obj,(WR_HPP+i)->Raw_str); 
	   }
     
     printf("Building Share Hash Memory Rawdata.OK!\n");
     
     shmdt(WR_HPP);
  	 free(tmpstr); 
  	 fclose(fp);
  	 printf("Create Hash Memory DB,OK!DBID=%d\n",shm_id);

	
}

void ShowHashTable(HashTb *SH_HPP)
{
	unsigned long i=0;
	for(i=0;i<HashTbSize;i++)
	  {
	  	if(i%8==0)
	  		{
	  			printf("\n");
	  		}
	  	printf("%3X:%11s* ",i,(*(SH_HPP+i)).Raw_obj);
	  }
	
}




//unsigned long *prepareCryptTable()
void prepareCryptTable()
{
	 //unsigned long *p;
   unsigned long seed=0x111111,index1=0,index2=0,i;
   
   for(index1=0;index1<0x100;index1++)
    {
    	//printf("------------\n");
      for(index2=index1,i=0;i<5;i++,index2 +=0x100)
         {  
						//printf("------------\n");
         	  unsigned long tmp1,tmp2;
	    
            seed=(seed*125+3)%0x2AAAAF;
            //printf("1-seed:%-6X:",seed);
            tmp1=(seed & 0xFFFF) << 0x10;
            
            seed=(seed*125+3)%0x2AAAAF;
            //printf("2-seed:%-6X:",seed);
 	          tmp2=(seed & 0xFFFF);
 	          
           	//printf("index1=%-5X,index2=%-5X,i=%d,tmp1=%12X,tmp2=%12X,arryvalue=%12X\n",index1,index2,i,tmp1,tmp2,tmp1|tmp2);
            crytable[index2]=(tmp1 | tmp2);
            //printf("%4d-%-10X\n",index2,crytable[index2]);
					}  

    }
    //p=&crytable[0];
    //return p;
}



int ShowCryptTable()
{
	int i=0;
	if(sizeof(crytable[0])==0)
   {
   	printf("CryptTable hasn't generated\n");
   	exit(0);
   }
  else
  	{
		for(;i<0x500;i++)
    {
      if(i%8==0)
       {
         printf("\n");
		     }
      printf("%4d-%-10X",i,crytable[i]); 
	   }
	  }
}


unsigned long HashString(char* lpszFileName,unsigned long dwHashType)
{
   unsigned char *key=(unsigned char*)lpszFileName;
   unsigned long seed1=0x7ED7FED;
   unsigned long seed2=0xEEEEEEE;
   int ch;
    
   //printf("\n\n");
 
   while(*key!=0)
   {
     ch=toupper(*key++);
     //printf("crytable arr:%X,crytable index:%X,dwhtype:%X,ch:%X,seed1:%X,seed2:%X,s1+s2:%X\n",crytable[(dwHashType << 8)+ch],(dwHashType << 8)+ch,dwHashType<<8,ch,seed1,seed2,seed1+seed2);
     seed1=crytable[(dwHashType << 8)+ch]^(seed1+seed2);
     //printf("seed1=%X\n",seed1);
     seed2=ch+seed1+seed2+(seed2<<5)+3;
     //printf("seed2=%X\n",seed2);
      }
  
   return seed1; 


}

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
}
