#include "tools.h"
#include <map>
#include <sstream>

using namespace std;

map<string,unsigned int> wc_map;

int main()
{
	std::ios::sync_with_stdio(false);
	string line;
	while(getline(cin,line))
	{
		 istringstream strstm(line);	
		 string str_frag;
		 while(strstm>>str_frag) 
		 {
			wc_map[str_frag]++;
			cout<<str_frag<<" "<<wc_map[str_frag]<<endl;
		 }
	}
}
