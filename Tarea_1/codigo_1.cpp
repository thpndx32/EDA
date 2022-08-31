#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <time.h>
#include <string>
#include <bits/stdc++.h>

using namespace std;

int rand1000(){
  //srand(time(NULL));
  return rand()%1000;
}

#define LIMIT 10000
#define SIZE 10

template <typename T,typename R>
R dis_n (vector<T> *arr,vector <T> *arr1){
  R temp=0;
  for (int i = 0; i<arr->size();i++){
    //cout<< arr[i]<< arr1[i]<<" "<<arr[i]-arr1[i]<<endl;
    temp+=pow((*arr)[i]-(*arr1)[i],2);
    //cout <<temp<<endl;
  }
  temp = sqrt(temp);
  //cout <<temp<<endl;
  return temp;
}

template <typename T, typename R>
void diezk(int x, int y){
  for(int j = 1; j <= x; j++){
    for (int k = 1; k<=y; k++){
      srand(time(NULL));
      vector <vector<T>*> cont (LIMIT*j);
      map <R,T> M;
      ofstream myFile;
      vector <R> temp;
      for_each(cont.begin(),cont.end(),[&k](vector<T>*&a) {
        vector <T> *b = new vector<T> (SIZE*k);
        generate(b->begin(),b->end(),rand1000);
        a = b;
        //copy(a->begin(),a->end(),ostream_iterator<T>(cout,"\n"));
        //cout << endl;
        });
      //copy(cont[0]->begin(),cont[0]->end(),ostream_iterator<T>(cout,"\n"));
      //cout << endl;
      //copy(cont[1]->begin(),cont[1]->end(),ostream_iterator<T>(cout,"\n"));
      for_each(cont.begin(),cont.end(),[&cont, &temp](vector<T> *a){
        temp.push_back ((round(dis_n<T,R>((cont[0]),a)*10.))/10.);
        //cout<<fixed<<setprecision(1)<<dis_n<T,R>((cont[0]),a)<<endl;
        });

      //copy(temp.begin(),temp.end(),ostream_iterator<float>(cout,"\n"));
      string b =  to_string(j)+'_'+to_string(k);
      string a = "export_" +b+".csv";
      myFile.open(a);


      for (int i = 0;i<temp.size();i++){
        if (M.find(temp[i]) == M.end()) {
                M[temp[i]] = 1;
            }
            else {
                M[temp[i]]++;
            }
      }

      for (auto& it : M) {
            myFile << it.first <<","<< it.second <<"\n";
      }
      myFile.close();
    }
  }

}

int main() {
  /*vector<int> arr (5), arr1 (5);
  generate(arr.begin(),arr.end(),rand1000);
  generate(arr1.begin(),arr1.end(),rand1000);
  copy(arr.begin(),arr.end(),ostream_iterator<int>(cout,"\n"));
  cout <<endl;
  copy(arr1.begin(),arr1.end(),ostream_iterator<int>(cout,"\n"));
  cout << dis_n<int,float>(&arr,&arr1);*/
  diezk<int,float>(5,5);
}
