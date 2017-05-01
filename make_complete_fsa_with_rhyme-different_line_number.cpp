#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<map>
#include<math.h>
#include<set>

using namespace std;


//ifstream fsa_without;


map<int,int> rhyme_start;
int tempnumber=0;


int give_me_tempnumber(){
	tempnumber++;
	return tempnumber-1;
}
void read_rhymes(){
	string pattern,word;
	int number=0;
	while(cin>>pattern>>word){
		
		//cout<<word<<endl;
		int l=pattern.length()/2+1;
		if(pattern[pattern.length()-1]=='0'){
			//cout<<"check the patterns ... end with 0 :" <<pattern<<endl;
			l=l-1;
		}
		int end=(number+1)*10;
		int start=(number+1)*10-l;
		rhyme_start[number]=start;
		
		string cur_word="";
		int cur_temp=-1;
		for(int i=word.length()-1;i>=0;i--){
			char c=word[i];
			if(c=='_'){
				if(cur_temp==-1){
					int t=give_me_tempnumber();
			//		cout<<t<<endl;
					cout<<"("<<end<<"-t ("<<"temp-"<<t<<" \""<<cur_word<<"\"))"<<endl;
					cur_temp=t;
				}
				else{
					int t=give_me_tempnumber();
					cout<<"("<<"temp-"<<cur_temp<<" ("<<"temp-"<<t<<" \""<<cur_word<<"\"))"<<endl;
					cur_temp=t;
				}
				
				cur_word="";
			}
			else{
				cur_word=c+cur_word;
			}	
		}
		
		if(cur_temp==-1){
			int t=give_me_tempnumber();
			cout<<"("<<end<<"-t ("<<start<<" \""<<cur_word<<"\"))"<<endl;
			cur_temp=t;
		}
		else{
			int t=give_me_tempnumber();
			cout<<"("<<"temp-"<<cur_temp<<" ("<<start<<" \""<<cur_word<<"\"))"<<endl;
			cur_temp=t;
		}
				
		
		//cout<<"("<<end<<"-t ("<<start<<" \""<<word<<"\"))"<<endl;
		number++;
	}
	
}
bool check_dist_with_rhyme(int start,int end){
	int ln=start/10;
	if(end>rhyme_start[ln])
		return false;
	return true;
}

int main(int argc, char** argv){
	//cout<<"I am here"<<endl;
	read_rhymes();
	
	
	
	
	return 0;
}
