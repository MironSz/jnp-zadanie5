#ifndef JNP_ZADANIE5_KEYED_QUEUE1_H
#define JNP_ZADANIE5_KEYED_QUEUE1_H

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

    using pairKV = std::pair <std::shared_ptr<K>, std::shared_ptr<V>>;
    using itKV = typename std::list<pairKV>::iterator;
    using list_of_iterators = std::list<itKV>;

    struct cmp_map {
        bool operator()(const std::shared_ptr <K> k1, const std::shared_ptr <K> k2) const {
            return *k1 < *k2;
        }
    };
    struct cmp_set {
        bool operator()(const itKV it1, const itKV it2) const {
            if(*it1->first == *it2->first)
                return *it1->second < *it2->second;
            return *it1->first < *it2->first;
        }
    };

    using map_key_to_list_of_occurances  = std::map <std::shared_ptr<K>, list_of_iterators, cmp_map>;

    bool shallow_copy_enable = true;

    //std::shared_ptr<std::set<itKV, cmp_set>> change_set;
    std::set<itKV, cmp_set> change_set;

    std::shared_ptr <std::list<pairKV>> list_of_pairs;
    std::shared_ptr <map_key_to_list_of_occurances> map_of_iterators;

    //dodana funkcja pomocnicza do oddawania referencji
    //rozumiem, ze w metodach gdie jest V const & nie ma sensu tego robić
    void change_single(itKV it) {
      shallow_copy_enable= false;
      std::cout << "change single: " << it->second.use_count() << "\n";
      if(list_of_pairs.unique() == false)
          full_copy();
      std::shared_ptr<K> k_ptr = it->first;
      std::shared_ptr<V> v_ptr = it->second;
      *it = {std::make_shared<K>(*k_ptr), std::make_shared<V>(*v_ptr)};

      //change_set->insert(it);
      change_set.insert(it);

      // for(auto i = change_set.begin(); i != change_set.end(); ++i)
      //     std::cout << *(*i)->first << " " << *(*i)->second. << " | ";
      // std::cout << "\n";
    }


    void full_copy() {
      std::cout << "PELNA KOPIA\n";
        std::list < pairKV > empty_list;
        std::shared_ptr <std::list<pairKV>> new_list = std::make_shared < std::list < pairKV >>(empty_list);

        for(auto it = list_of_pairs->begin();it!=list_of_pairs->end();it++){
            new_list->push_back(*it);
        }
        map_key_to_list_of_occurances new_map;

        for (itKV it = new_list->begin(); it != new_list->end(); ++it) {
            if (new_map.find(it->first) == new_map.end()) {
                new_map.insert(std::make_pair(it->first, list_of_iterators{}));
            }
            new_map[it->first].push_back(it);
        }
        //TODO napisz najpierw stworzenie shared_ptr potem przypisz
        auto help = std::make_shared<map_key_to_list_of_occurances>(new_map);
        swap(list_of_pairs,new_list);
        map_of_iterators = help;
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
        std::cout << "W UZYCIU: " << old_queue.list_of_pairs.use_count()
                  << ", " << old_queue.map_of_iterators.use_count() << "\n";
        new_queue.list_of_pairs = old_queue.list_of_pairs;
        new_queue.map_of_iterators = old_queue.map_of_iterators;
        //TODO licznik
        //poprawka z shallow_copy_enable na old_queue.shallow_copy_enable
        if (old_queue.shallow_copy_enable == false) {
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

        auto found = map_ptr->find(k_ptr);
        std::shared_ptr <V> v_ptr = std::make_shared<V>(v);

        if (found != map_ptr->end()) {
            k_ptr = found->first;
        }

        std::list <pairKV> singleton({std::make_pair(k_ptr, v_ptr)});
        std::list <itKV> singletonIt({singleton.begin()});

        if (found == (map_ptr)->end()) {
            auto ret = (map_ptr)->insert(
                    std::make_pair(k_ptr, list_of_iterators{}));
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

        list_of_pairs->splice(list_of_pairs->end(), singleton);
        found->second.splice((found->second).end(), singletonIt);

        auto i = list_of_pairs->begin();
        while (i != list_of_pairs->end()) {
            std::cout << *i->first << " " << *i->second << " | ";
            ++i;
        }
        std::cout << "\n";
    }

    void pop() {
        if (empty()) {
            throw lookup_error();
        }
        pop(*list_of_pairs->front().first);
    }

    void pop(K const &k) {

        //DO DOPISANIA: usuwanie z change_set


        std::cout << "POP: " << k << "\n";
        auto k_ptr = std::make_shared<K>(k);
        auto map_ptr = map_of_iterators;
        auto list_ptr = list_of_pairs;
        bool wasnt_unique = false;

        if (map_of_iterators.use_count() > 2) {
            std::cout << "Bedziemy kopiowac\n";
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

        if (wasnt_unique) {
            map_of_iterators = map_ptr;
            list_of_pairs = list_ptr;
        }

        itKV it = found->second.front();
        list_of_pairs->erase(it);
        found->second.erase(found->second.begin());

        if (found->second.empty()) {//TODO upewnic sie, ze nie rzuca
            map_of_iterators->erase(k_ptr);
        }

        auto i = list_of_pairs->begin();
        while (i != list_of_pairs->end()) {
            std::cout << *i->first << " " << *i->second << " | ";
            ++i;
        }
        std::cout << "\n";
    }

    void move_to_back(K const &k) {
        std::cout << "MOVE TO BACK: " << k << "\n";

        auto k_ptr = std::make_shared<K>(k);
        auto found = map_of_iterators->find(k_ptr);

        if (found == map_of_iterators->end()) {
            throw lookup_error();
        }

        std::list <pairKV> new_list_of_pairs;
        std::list <itKV> new_list_of_iterators;

        for (auto i = found->second.begin(); i != found->second.end(); ++i) {
            new_list_of_pairs.push_back(**i);
        }

        for (auto i = new_list_of_pairs.begin(); i != new_list_of_pairs.end(); ++i) {
            new_list_of_iterators.push_back(i);
        }
        std::cout << "MOve to back, use_count " << map_of_iterators.use_count() << "\n";
        if (map_of_iterators.unique() == false) {
            keyed_queue<K, V> new_queue(*this);
            new_queue.full_copy();
            found = new_queue.map_of_iterators->find(k_ptr);
            map_of_iterators = new_queue.map_of_iterators;
            list_of_pairs = new_queue.list_of_pairs;

            found = map_of_iterators->find(k_ptr);//TODO usun
        }
//Od teraz no-throw
        while (found->second.empty() == false) {
            std::cout<<"usuwam "<< *found->second.front()->first <<"  "<<*found->second.front()->second<<"\n";
            list_of_pairs->erase(found->second.front());
            found->second.pop_front();
        }
        std::cout<<"rozmiar dodawanego splicem:"<<new_list_of_pairs.size()<<"\n";
        list_of_pairs->splice(list_of_pairs->end(), new_list_of_pairs);
        swap(found->second, new_list_of_iterators);
        auto it = list_of_pairs->begin();
        while (it != list_of_pairs->end()) {
            std::cout << *it->first << " " << *it->second << " | ";
            ++it;
        }
        std::cout << "\n";
    }

    std::pair<K const &, V &> front() {
        if (empty()) {
            throw lookup_error();
        }

        change_single(list_of_pairs->begin());

        K const &key = *list_of_pairs->front().first;
        V &val = *list_of_pairs->front().second;
        return {key, val};
    };

    std::pair<K const &, V &> back() {
        if (empty()) {
            throw lookup_error();
        }

        change_single(--list_of_pairs->end());

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
        auto k_ptr = std::make_shared<K>(key);
        auto found = map_of_iterators->find(k_ptr);
        if (found == map_of_iterators->end()) {
            throw lookup_error();
        }

        change_single(found->second.front());
        found = map_of_iterators->find(k_ptr);

        K const &k = *found->second.front()->first;
        V &val = *found->second.front()->second;
        return {k, val};
    };

    std::pair<K const &, V &> last(K const &key) {
        auto k_ptr = std::make_shared<K>(key);
        auto found = map_of_iterators->find(k_ptr);
        if (found == map_of_iterators->end()) {
            throw lookup_error();
        }

        change_single(found->second.back());
        found = map_of_iterators->find(k_ptr);

        K const &k = *found->second.back()->first;
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

#endif //JNP_ZADANIE5_KEYED_QUEUE1_H
