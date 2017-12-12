#include <iostream>
#include <map>
#include "keyed_queue.h"
using namespace std;
int main() {
  map<int, int> m;
  keyed_queue<int, int> kolejka = {};
  cout << kolejka.size() << "\n";
  kolejka.push(14, 3);
  kolejka.push(2, 2);
  kolejka.push(14, 6);
  //kolejka.pop(2);
  kolejka.push(14, 10);
  kolejka.pop();
  kolejka.push(2, 1);
  cout << kolejka.size() << "\n";
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
