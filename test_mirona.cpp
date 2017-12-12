//
// Created by miron on 12/9/17.
//
#include "keyed_queue.h"
#include <iostream>

int main(){
    keyed_queue<int,int> queue;
    queue.push(1,3);
    keyed_queue<int,int>::k_iterator it = queue.k_begin();
    std::cout << *it <<std::endl;
//    *it;
    queue.pop();

}