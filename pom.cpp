#include <iostream>
#include <map>
#include "keyed_queue.h"
using namespace std;
int main() {
  map<int, int> m;
  keyed_queue<int, int> kolejka = {};
  cout << kolejka.size() << "\n";
  kolejka.push(100, 1);
  kolejka.push(200, 1);
  kolejka.push(200, 2);
  kolejka.push(100, 2);
//  kolejka.pop(200);
  kolejka.pop();
  kolejka.pop(200);
  kolejka.pop(100);
//  kolejka.pop(2);
  cout <<" * z maina\n";
  kolejka.push(100, 3);
  //kolejka.pop();
  cout << kolejka.size() << "\n";
  for(auto b = kolejka.k_begin(); b != kolejka.k_end(); ++b)
    cout << *b << " | ";
  cout << "\n";
  cout << kolejka.front().second << "\n";
  cout << kolejka.back().second << "\n";
  cout << kolejka.first(100).second << "\n";
  cout << kolejka.last(200).second << "\n";
//  cout<<"kopiujemy\n";
//  auto kolejka2(kolejka);
//  kolejka.push(3,1);
//  for(auto b = kolejka2.k_begin(); b != kolejka2.k_end(); ++b)
//    cout << *b << " | ";
//  cout << "\n";
//  for(auto b = kolejka.k_begin(); b != kolejka.k_end(); ++b)
//    cout << *b << " | ";
//  cout << "\n";
  // //kolejka.front();
  // auto a = kolejka.front();
  // cout << a.first << " " << a.second << "\n";
  // a.second = 123;
  //
  // auto b = kolejka.front();
  // cout << b.first << " " << b.second << "\n";
  //
  // auto c = kolejka.first(14);
  //   cout << c.first << " " << c.second << "\n";
  // c.second = 1;
  //
  // kolejka.last(2).second = 15;
  // cout << kolejka.back().second << "\n";
}
