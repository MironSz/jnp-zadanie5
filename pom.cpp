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
  kolejka.pop(2);
  kolejka.push(14, 10);
  kolejka.pop();
  kolejka.push(2, 1);
  cout << kolejka.size() << "\n";
  for(auto b = kolejka.k_begin(); b != kolejka.k_end(); ++b)
    cout << *b << " | ";
  cout << "\n";
  //kolejka.front();
}
