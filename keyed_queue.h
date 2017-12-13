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
        bool operator()(std::shared_ptr <K> k1, std::shared_ptr <K> k2) {
            return *k1 < *k2;
        }
    };

    using pairKV = std::pair <std::shared_ptr<K>, std::shared_ptr<V>>;
    using itKV = typename std::list<pairKV>::iterator;
    using list_of_iterators = std::list<itKV>;
    using map_key_to_list_of_occurances  = std::map <std::shared_ptr<K>, list_of_iterators, cmp>;

    bool shallow_copy_enable = true;

    std::shared_ptr <std::list<pairKV>> list_of_pairs;
    std::shared_ptr <map_key_to_list_of_occurances> map_of_iterators;

    void full_copy() {
        auto new_list = std::list<pairKV>(*list_of_pairs);

        map_key_to_list_of_occurances new_map;

        for (itKV it = new_list.begin(); it != new_list.end(); ++it) {
            if (new_map.find(it->first) == new_map.end()) {
                new_map.insert(std::make_pair(it->first, list_of_iterators{}));
            }
            new_map[it->first].push_back(it);
        }
        //TODO napisz najpierw stworzenie shared_ptr potem przypisz
        list_of_pairs = std::make_shared < std::list < pairKV >> (new_list);
        map_of_iterators = std::make_shared<map_key_to_list_of_occurances>(new_map);
    }

    class k_iterator {
    private:
        typename map_key_to_list_of_occurances::const_iterator i;

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
        auto new_list = std::make_shared < std::list < pairKV >> (std::list < pairKV > {});
        auto new_map = std::make_shared<map_key_to_list_of_occurances>(map_key_to_list_of_occurances{});
        list_of_pairs = new_list;
        map_of_iterators = new_map;
        shallow_copy_enable = true;
        //TODO licznik
    }

    keyed_queue(keyed_queue const &old_queue) {
        keyed_queue new_queue;
        new_queue.list_of_pairs = old_queue.list_of_pairs;
        new_queue.map_of_iterators = old_queue.map_of_iterators;
        std::cout <<"Nowa kopia count:" <<old_queue.list_of_pairs.use_count()<<"\n";
        //TODO licznik
        if (shallow_copy_enable == false) {
            std::cout << "w srdku\n";
            new_queue.full_copy();
        }
        list_of_pairs = new_queue.list_of_pairs;
        map_of_iterators = new_queue.map_of_iterators;

        shallow_copy_enable = true;
    }

    keyed_queue(keyed_queue &&) = default;

    void push(K const &k, V const &v) {
        std::shared_ptr <K> k_ptr = std::make_shared<K>(k);//
        std::cout << "push: " << k << " " << v << "\n";
        auto map_ptr = map_of_iterators;
        auto list_ptr = list_of_pairs;
        bool wasnt_unique = false;
        if (map_of_iterators.use_count() > 2) {
            wasnt_unique = true;
            keyed_queue<K, V> new_queue(*this);
            new_queue.full_copy();//TODO moze byc podwojna kopia
            map_ptr = new_queue.map_of_iterators;
            list_ptr = new_queue.list_of_pairs;
        }

        auto found = map_ptr->find(k_ptr);//founs jest para (key,list<iterator>)
        std::shared_ptr <V> v_ptr = std::make_shared<V>(v);//

        if (found != map_ptr->end()) {//Jesli znalezlismy w mapie dany klucz
            k_ptr = found->first;
        }

        std::list <pairKV> singleton({std::make_pair(k_ptr, v_ptr)});//
        std::list<itKV> singletonIt({singleton.begin()});//

        if (found == (map_ptr)->end()) {
            auto ret = (map_ptr)->insert(
                    std::make_pair(k_ptr, list_of_iterators{}));//JEdyna funnkcja ktora moze wywalic i modyfikuje
            if (ret.second == true) {
                found = ret.first;
            } else {
                //TODO Czy insert moe rzucic wyjatek alokacji
            }
        }

        if (wasnt_unique) {
            map_of_iterators = map_ptr;
            list_of_pairs = list_ptr;
        }

        list_of_pairs->splice(list_of_pairs->end(),singleton);
        found->second.splice((found->second).end(),singletonIt);
//        auto it = list_of_pairs->begin();
//        while(it != list_of_pairs->end()) {
//            std::cout << *it->first << " " << *it->second << " | ";
//            ++it;
//        }
//        std::cout << "\n";
    }

    void pop() {
        if (empty()) {
            throw lookup_error();
        }
        pop(*list_of_pairs->front().first);
    }

    void pop(K const &k) {
        auto k_ptr = std::make_shared<K>(k);
        auto map_ptr = map_of_iterators;
        auto list_ptr = list_of_pairs;
        bool wasnt_unique = false;
        if (map_of_iterators.use_count() > 2) {
            wasnt_unique = true;
            keyed_queue<K, V> new_queue(*this);
            new_queue.full_copy();//TODO moze byc podwojna kopia
            map_ptr = new_queue.map_of_iterators;
            list_ptr = new_queue.list_of_pairs;
        }

        auto found = map_ptr->find(k_ptr);//
        if (found == map_ptr->end()) {
            throw lookup_error();
        }

        if(wasnt_unique) {
            map_of_iterators = map_ptr;
            list_of_pairs = list_ptr;
        }

        itKV it = found->second.front();
        ++it;
        std::cout << *it->first << " " << *it->second << "\n";
        list_of_pairs->erase(it);
        std::cout << "*\n";
        found->second.erase(found->second.begin());

        if (found->second.empty()) {//TODO upewnic sie, ze nie rzuca
            map_of_iterators->erase(k_ptr);
        }
        std::cout << "POP: " << k << "\n";
        auto i = list_of_pairs->begin();
        while(i != list_of_pairs->end()) {
            std::cout << *i->first << " " << *i->second << " | ";
            ++i;
        }
        std::cout << "\n";
    }

    void move_to_back(K const &k) {
        auto k_ptr = std::make_shared<K>(k);
        auto found = map_of_iterators->find(k_ptr);
        if (found == map_of_iterators->end()) {
            throw lookup_error();
        }

        std::list<pairKV> new_list_of_pairs;
        std::list<itKV> new_list_of_iterators;

        for(auto i = found->second.begin(); i!=found->second.end(); ++i) {
            new_list_of_pairs.push_back(**i);
        }

        for(auto i = new_list_of_pairs.begin(); i!=new_list_of_pairs.end(); ++i){
            new_list_of_iterators.push_back(i);
        }
//        if(should_copy){}
        auto new_queue = *this;
        new_queue.full_copy();
//Od teraz no-throw
        while(found->second.empty() == false) {
            list_of_pairs->erase(found->second.front());
            found->second.pop_front();
        }

        list_of_pairs->splice(list_of_pairs->end(),new_list_of_pairs);
        found->second.splice(found->second.end(),new_list_of_iterators);
        std::cout << "MOVE TO BACK: " << k << "\n";
        auto it = list_of_pairs->begin();
        while(it != list_of_pairs->end()) {
            std::cout << *it->first << " " << *it->second << " | ";
            ++it;
        }
        std::cout << "\n";
    }

    std::pair<K const &, V &> front() {
        if (empty()) {
            throw lookup_error();
        }
        K const &key = *list_of_pairs->front().first;
        V &val = *list_of_pairs->front().second;
        return {key, val};
    };

    std::pair<K const &, V &> back() {
        if (empty()) {
            throw lookup_error();
        }
        K const &key = *list_of_pairs->back().first;
        V &val = *list_of_pairs->back().second;
        return {key, val};
    };

    std::pair<K const &, V const &> front() const {
        if (empty()) {
            throw lookup_error();
        }
        K const &key = *list_of_pairs->front().first;
        V const &val = *list_of_pairs->front().second;
        return {key, val};
    };

    std::pair<K const &, V const &> back() const {
        if (empty()) {
            throw lookup_error();
        }
        K const &key = *list_of_pairs->back().first;
        V const &val = *list_of_pairs->back().second;
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
        if (found == map_of_iterators->end()) {
            throw lookup_error();
        }
        V &val = *found->second.back()->second;
        return {key, val};
    };

    std::pair<K const &, V const &> first(K const &key) const {
        auto found = map_of_iterators->find(std::make_shared<K>(key));
        if (found == map_of_iterators->end()) {
            throw lookup_error();
        }
        V const &val = *found->second.front()->second;
        return {key, val};
    };

    std::pair<K const &, V const &> last(K const &key) const {
        auto found = map_of_iterators->find(std::make_shared<K>(key));
        if (found == map_of_iterators->end()) {
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
