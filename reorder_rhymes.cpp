#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>




using namespace std;

int main(){
   vector<string> rhymes;
   int number_rhyme=0;

   string s;
   while(getline(cin,s)){
   	rhymes.push_back(s);
   	number_rhyme++;
   }
   //cout<<number_rhyme<<endl;
      //swap 0,number_rhyme-2
      //swap 1,number_rhyme-1
   
   string t="";
   
   t=rhymes[0];
   rhymes[0]=rhymes[number_rhyme-2];
   rhymes[number_rhyme-2]=t;

   t=rhymes[1];
   rhymes[1]=rhymes[number_rhyme-1];
   rhymes[number_rhyme-1]=t;

  if(number_rhyme==2){
      cout<<rhymes[0]<<endl;
      cout<<rhymes[1]<<endl;
      return 0;
  }
   for(int i=0;i<min(number_rhyme,12);i++){
   		if(i%4==0)
   			cout<<rhymes[i]<<endl;
   		else if(i%4==1)
   			cout<<rhymes[i+1]<<endl;
   		else if(i%4==2)
   			cout<<rhymes[i-1]<<endl;
   		else
   			cout<<rhymes[i]<<endl;
   }
   if(number_rhyme==14){
   	cout<<rhymes[12]<<endl;
   	cout<<rhymes[13]<<endl;
   	
   }


	return 0;
}
