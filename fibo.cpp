#include <iostream>
#include <string>
#include <chrono>
using namespace std;
using namespace std::chrono;

int fib_aux[2];

int mat [2][2] = {{0,1},{1,1}};
int fibo_m(int n){
  if (n == 1) {
    fib_aux[0] = 0;
    fib_aux[1] = 1;
    return fib_aux[1];
  }
  fibo_m(n-1);
  int temp=fib_aux[0];
  fib_aux[0] = mat[0][0]*temp+mat[1][0]*fib_aux[1];
  fib_aux[1] = mat[1][0]*temp+mat[1][1]*fib_aux[1];
  return fib_aux[1];
}

int fibo(int n){
  if (n==1) return 1;
  else if (n==0) return 0;
  return fibo(n-1)+fibo(n-2);
}

int fibo_l(int n,int &temp){
  if (n == 1) {
    temp = 0;
    return 1; 
    }
  int x = fibo_l(n-1,temp);
  int temp1 = x;
  x = x +temp;
  temp=temp1;
  //cout<<x<<" "<<temp<<endl;
  return x;
};

int main() {
  /*string temp;
  cin >> temp;
  while(1) {
    
    cout<<temp;
  }*/
  int temp =0;
  int x=40;
  auto start = high_resolution_clock::now();
  cout<<fibo_m(x)<<endl;
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << duration.count() << endl;
  auto start2 = high_resolution_clock::now();
  cout<<fibo_l(x,temp)<<endl;
  auto stop2 = high_resolution_clock::now();
  auto duration2 = duration_cast<microseconds>(stop2 - start2);
  cout << duration2.count() << endl;
  /*auto start = high_resolution_clock::now();
  cout<<fibo_m(x)<<endl;
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << duration.count() << endl;*/
  auto start1 = high_resolution_clock::now();
  cout<<fibo(x)<<endl;
  auto stop1 = high_resolution_clock::now();
  auto duration1 = duration_cast<microseconds>(stop1 - start1);
  cout << duration1.count() << endl;
}