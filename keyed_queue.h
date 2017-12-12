#ifndef JNP_ZADANIE5_KEYED_QUEUE_H
#define JNP_ZADANIE5_KEYED_QUEUE_H

#include <stdlib.h>
#include <utility>
#include <list>
#include <memory>
#include <set>
#include <map>

class lookup_error : public std::exception {
    virtual const char *what() const throw() {
        return "Lookup error";
    }
};

template<class K, class V>
class keyed_queue {
private:

    struct cmp {
        bool operator() (std::shared_ptr<K> k1, std::shared_ptr<K> k2) {
            return *k1 < *k2;
        }
    };

    using pairKV = std::pair<std::shared_ptr<K>, std::shared_ptr<V>>;
    using itKV = typename std::list<pairKV>::iterator;
    using list_of_itarator = std::list<itKV>;
    using map_key_to_list_of_occurances  = std::map <std::shared_ptr<K>, list_of_itarator, cmp>;
    bool shallow_copy_enable;
    std::shared_ptr<std::list<pairKV>>  list_of_pairs;

    std::shared_ptr<map_key_to_list_of_occurances> map_of_iterators;

    void full_copy(){

        auto new_list = std::list<pairKV>(*list_of_pairs.get());

        map_key_to_list_of_occurances new_map;

        for(itKV it = new_list.begin();it != new_list.end();++it){
            if(new_map.find(it->first) == new_map.end()){
                new_map.insert(std::make_pair(it->first,list_of_itarator{}));
            }
            new_map[it->first].push_back(it);
        }

        list_of_pairs = std::make_shared<std::list<pairKV>>(new_list);
        map_of_iterators = std::make_shared<map_key_to_list_of_occurances>(new_map);
    }

    class k_iterator {
    private:
        typename map_key_to_list_of_occurances ::const_iterator i;

    public:
        k_iterator(typename map_key_to_list_of_occurances::const_iterator it) : i(it) {};

        k_iterator &operator++() {
            ++i;
            return *this;
        }

        bool operator==(const k_iterator k) const {
            return i == k.i;
        }

        bool operator!=(const k_iterator k) const {
            return i != k.i;
        }

        K operator*() {
            return *(i->first.get());
        }
    };

public:
    k_iterator k_begin() {
        return k_iterator(map_of_iterators->begin());
    }

    k_iterator k_end() {
        return k_iterator(map_of_iterators->end());
    }

    keyed_queue() {
        list_of_pairs = std::make_shared<std::list<pairKV>>(std::list<pairKV>{});
        map_of_iterators = std::make_shared<map_key_to_list_of_occurances>(map_key_to_list_of_occurances{});
        shallow_copy_enable = true;
        //TODO licznik
    }

    keyed_queue(keyed_queue const &old_queue) {
        list_of_pairs = old_queue.list_of_pairs;
        map_of_iterators = old_queue.map_of_iterators;
        //TODO licznik
        if(shallow_copy_enable == false){
            full_copy();
        }
        shallow_copy_enable = true;
    }

    keyed_queue(keyed_queue &&) = default;

    void push(K const &k, V const &v) {
        std::shared_ptr<K> k_ptr = std::make_shared<K>(k);//

        auto map_ptr = map_of_iterators;
        auto list_ptr = list_of_pairs;
        bool was_unique = false;
        if(map_of_iterators.unique() == false){
            was_unique = true;
            keyed_queue<K,V> new_queue(*this);
            new_queue.full_copy();//TODO moze byc podwojna kopia
            map_ptr = new_queue.map_of_iterators;
            list_ptr = new_queue.list_of_pairs;
        }

        auto found = map_ptr->find(k_ptr);//founs jest para (key,list<iterator>)
        std::shared_ptr<V> v_ptr = std::make_shared<V>(v);//

        if(found != map_ptr->end()){//Jesli znalezlismy w mapie dany klucz
            k_ptr = found->first;
//            std::cout << "znal " << *k_ptr << " " << *(found->second.front())->second << "\n";
        }

        std::list<pairKV> singleton({std::make_pair(k_ptr,v_ptr)});//

        if (found == (map_ptr)->end()) {
            std::cout<<"powinno byc true:";
            auto ret = (map_ptr)->insert(std::make_pair(k_ptr, list_of_itarator{}));//JEdyna funnkcja ktora moze wywalic i modyfikuje
            if(ret.second == true){
                std::cout<<"jest\n";
                found = ret.first;
            } else{
                //TODO
            }
        } else{
            for(auto i:*list_of_pairs){
                std::cout<<"("<<*i.first<<","<<*i.second<<") ";
            }
            std::cout<<"\n";
            //              dodawane key i val      znalezione key         znaleziony klucz iteratora       znaleziony val iterator
                std::cout << k<<" "<<v<<" |  " << *found -> first << " "
                          <<  *(found->second.front()->first)<<"  "<<*(found->second.front()->second) << "             koniec\n";
        }

        if(was_unique){
            map_of_iterators = map_ptr;
            list_of_pairs = list_ptr;
        }

        (list_of_pairs)->push_back({k_ptr, v_ptr});//splice(list_of_pairs->end(),singleton);
//        (list_of_pairs)->splice(list_of_pairs->end(),singleton);
        auto help=list_of_pairs->end();
        --help;
        (found->second).push_back(help);
//        std::cout << "Z tego klucza\n";
//        for(auto i : found->second) {
//            std::cout << *i->first << " " << *i->second << "**\n";
//            auto b = i;
//            ++b;
//            if(b != list_of_pairs->end()) {
//                std::cout<<"**\n";
//                std::cout << *b->first << " " << *b->second << "<-nastepny\n";
//            }
//        }
    }

    void pop() {
        if (empty()) {
            throw lookup_error();
        }
        pairKV p = list_of_pairs.front();

        map_of_iterators[*(p.first)].
                erase(map_of_iterators[*(p.first)].begin());

        list_of_pairs.pop_front();

        if (map_of_iterators[*(p.first)].empty()) {
            map_of_iterators.erase(*(p.first));
        }
    }

    void pop(K const &k) {
        auto k_ptr = std::make_shared<K>(k);
        auto found = map_of_iterators->find(k_ptr);//
        if (found == map_of_iterators->end()) {
            throw lookup_error();
        }

        itKV it = found->second.front();
        std::cout << *it->first << " " << *it->second << "\n";
        std::cout<<"++++++++++++++\n";
        while(it!=list_of_pairs->end()) {
            std::cout << *it->first <<"|"<<*it->second<<"\n";
            ++it;
        }
        std::cout<<"++++++++++++++\n";

        list_of_pairs->erase(++it);

        for(auto elem : *list_of_pairs) {
            std::cout << *elem.first <<"|"<<*elem.second<<"\n";
        }
        std::cout<<"++++++++++++++\n";

//        std::cout << found->
        found->second.erase(found->second.begin());

        if (found->second.empty()) {
            std::cout <<"*\n";
            map_of_iterators->erase(k_ptr);
        }
        std::cout <<"*\n";
    }

    void move_to_back(K const &k) {
        auto found = map_of_iterators.find(k);
        if (found == map_of_iterators.end()) {
            throw lookup_error();
        }

        list_of_itarator l = found->second;
        int number_of_elements_to_move = l.size();

        for (int i = 0; i < number_of_elements_to_move; i++) {
            itKV it = l.front();
            list_of_pairs.push_back(*it);
            list_of_pairs.erase(it);
            l.pop_front();
            l.push_back(list_of_pairs.end()--);
        }

        found->second = l;
    }

    std::pair<K const &, V &> front() {
        if (empty()) {
            throw lookup_error();
        }
        K const &key = list_of_pairs.front().first;
        V &val = list_of_pairs.front().second;
        return {key, val};
    };

    std::pair<K const &, V &> back() {
        if (empty()) {
            throw lookup_error();
        }
        K const &key = list_of_pairs.back().first;
        V &val = list_of_pairs.back().second;
        return {key, val};
    };

    std::pair<K const &, V const &> front() const {
        if (empty()) {
            throw lookup_error();
        }
        K const &key = list_of_pairs.front().first;
        V const &val = list_of_pairs.front().second;
        return {key, val};
    };

    std::pair<K const &, V const &> back() const {
        if (empty()) {
            throw lookup_error();
        }
        K const &key = list_of_pairs.back().first;
        V const &val = list_of_pairs.back().second;
        return {key, val};
    };

    std::pair<K const &, V &> first(K const &key) {
        auto found = map_of_iterators->find(std::make_shared<K>(key));
        if (found == map_of_iterators->end()) {
            throw lookup_error();
        }
        V &val = *found->second.front()->second;
        return {key, val};
    };

    std::pair<K const &, V &> last(K const &key) {
        auto found = map_of_iterators->find(std::make_shared<K>(key));
        if (found == map_of_iterators.end()) {
            throw lookup_error();
        }
        V &val = *found->second.back()->second;
        return {key, val};
    };

    std::pair<K const &, V const &> first(K const &key) const {
        auto found = map_of_iterators->find(std::make_shared<K>(key));
        if (found == map_of_iterators.end()) {
            throw lookup_error();
        }
        V const &val = *found->second.front()->second;
        return {key, val};
    };

    std::pair<K const &, V const &> last(K const &key) const {
        auto found = map_of_iterators->find(std::make_shared<K>(key));
        if (found == map_of_iterators.end()) {
            throw lookup_error();
        }
        V const &val = *found->second.back()->second;
        return {key, val};
    };

    size_t size() const {
        return list_of_pairs.get()->size();
    }

    bool empty() const {
        return size() == 0;
    }

    void clear() {

    }

    size_t count(K const &k) const {
        if (map_of_iterators.find(k) == map_of_iterators.end()) {
            throw lookup_error();
        }
        return map_of_iterators[k].size();
    }
};

#endif //JNP_ZADANIE5_KEYED_QUEUE_H
