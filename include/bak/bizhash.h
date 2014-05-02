
typedef struct{               //4+4+4+12+50=74Bytes                    //HASH结构体
   unsigned long Hash1;
   unsigned long Hash2;
   int bExist;
   char Raw_obj[12];  
   char Raw_str[50];
} HashTb;



//生成KEY密码树1280[0x500]个
void prepareCryptTable();
//展示KEY密码树1280[0x500]个
int ShowCryptTable();
//计算字符串对应不同HASH类型的HASH值
unsigned long HashString(char* lpszfileName,unsigned long dwHashType);  
//链接到HASH内存库，映射至指针RO_HPP指针
void ConnMemHashTable(const char *SMemPath,int SMemFd); 
//创建内存库，映射至WR_HPP指针
void CrtMemHashTable(char *SMemPath,int SMemFd,char *RawFileName,unsigned long RawMemSize);
//展示HASH表全表
void ShowHashTable(HashTb *SH_HPP);
//定义非阻塞
void Setnonblocking(int sock);	




