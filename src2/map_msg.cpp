/* 从消息队列中收取数据，同时标准输出，用于map
*
*
*
*/
#include "tools.h"

using namespace std;

long num_call=0;

int main()
{
	int ret;
	int mq_id=StartMQ(0x77);
	cout<<mq_id<<endl;
	msg_t data;
	while(1)
	{
		ret=msgrcv(mq_id, (void *)&data,sizeof(msg_t),0,0);
		if(ret==-1)
        {
           ShowErr("msgrcv error");
        }
        cout<<num_call++<<":"<<data.type<<":"<<data.msg_text<<endl;
	}
}
