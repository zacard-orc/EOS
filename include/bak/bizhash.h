
typedef struct{               //4+4+4+12+50=74Bytes                    //HASH�ṹ��
   unsigned long Hash1;
   unsigned long Hash2;
   int bExist;
   char Raw_obj[12];  
   char Raw_str[50];
} HashTb;



//����KEY������1280[0x500]��
void prepareCryptTable();
//չʾKEY������1280[0x500]��
int ShowCryptTable();
//�����ַ�����Ӧ��ͬHASH���͵�HASHֵ
unsigned long HashString(char* lpszfileName,unsigned long dwHashType);  
//���ӵ�HASH�ڴ�⣬ӳ����ָ��RO_HPPָ��
void ConnMemHashTable(const char *SMemPath,int SMemFd); 
//�����ڴ�⣬ӳ����WR_HPPָ��
void CrtMemHashTable(char *SMemPath,int SMemFd,char *RawFileName,unsigned long RawMemSize);
//չʾHASH��ȫ��
void ShowHashTable(HashTb *SH_HPP);
//���������
void Setnonblocking(int sock);	




