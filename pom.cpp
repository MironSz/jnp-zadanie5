#include <iostream>
#include <map>
#include "keyed_queue.h"
#include <tuple>

using namespace std;
int main() {
    {



        map<int, int> m;
        keyed_queue<int, int> kolejka = {};
        cout << kolejka.size() << "\n";
        kolejka.push(100, 1);
        kolejka.push(200, 1);
        kolejka.push(200, 2);
        kolejka.push(100, 2);
        keyed_queue<int, int> kolejka1(kolejka);
        // kolejka.move_to_back(200);
        // kolejka1.push(123, 123);
        // kolejka.push(1, 1);
        kolejka.move_to_back(200);
        kolejka1.pop();
        kolejka.push(12, 123);
        cout << " * z maina\n";
//        keyed_queue kolejka1(kolejka);

//        kolejka1.push(200, 3);
//        kolejka.move_to_back(200);
//        kolejka.push(123, 1);
//        kolejka1.push(1, 1);
        //kolejka.pop();
//        cout << kolejka.size() << "\n";
//        for (auto b = kolejka.k_begin(); b != kolejka.k_end(); ++b)
//            cout << *b << " | ";

        // kolejka.pop();
//        kolejka.pop(200);
        cout << kolejka.front().first << "\n";
//        cout << kolejka1.front().first << "\n";

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

}
