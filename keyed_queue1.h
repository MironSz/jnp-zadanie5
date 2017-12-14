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
    //Attribute remembers if queue gave any non-const reference.
    bool shallow_copy_enable = true;
    //Set of 'given' values.
    std::set <std::shared_ptr<V>> change_set;
    //{shared pointer on key, shared pointer on value}
    std::shared_ptr <std::list<pairKV>> list_of_pairs;

    struct cmp_map {
        bool operator()(const std::shared_ptr <K> k1, const std::shared_ptr <K> k2) const {
            return *k1 < *k2;
        }
    };


    //Map assigning list of iterators itKV for every shared pointer on key.
    using map_key_to_list_of_occurances  = std::map <std::shared_ptr<K>, list_of_iterators, cmp_map>;
    std::shared_ptr <map_key_to_list_of_occurances> map_of_iterators;

    //DODANA: funkcja pomocnicza do oddawania referencji
    //rozumiem, ze w metodach gdie jest V const & nie ma sensu tego robić
    void change_single(itKV it) {
        shallow_copy_enable = false;
        auto help = std::make_shared<K>(*it->second);
        if (list_of_pairs.unique() == false) {
            auto new_queue = keyed_queue(*this);
            change_set.insert(help);
            this->map_of_iterators = new_queue.map_of_iterators;
            this->list_of_pairs = new_queue.list_of_pairs;
        } else {
            change_set.insert(help);
        }
        it->second = help;
    }
    bool gave_reference(std::shared_ptr<V> v_ptr){
        return change_set.find(v_ptr)!=change_set.end();
    }

    //Function that copies resources of queue, so it doesn't share its resources.
    void full_copy(keyed_queue<K,V> old_queue) {
        std::list <pairKV> empty_list;
        std::shared_ptr <std::list<pairKV>>
                new_list = std::make_shared < std::list < pairKV >> (empty_list);

        //Copying list_of_pairs
        for (auto it = list_of_pairs->begin(); it != list_of_pairs->end(); ++it) {
            //DODANE:
            auto found = change_set.find(it->second);
            if (found != change_set.end()) {
                new_list->push_back({it->first, std::make_shared<K>(*it->second)});
            } else {
                new_list->push_back(*it);
            }
        }
        change_set.clear();//TODO ??? KAdy ma wasny set, kiedy go czyscimy
        map_key_to_list_of_occurances new_map;
        //Copying map shared_ptr<K> -> list_of_iterators
        for (itKV it = new_list->begin(); it != new_list->end(); ++it) {
            if (new_map.find(it->first) == new_map.end()) {
                new_map.insert(std::make_pair(it->first, list_of_iterators{}));
            }
            new_map[it->first].push_back(it);
        }
        auto help = std::make_shared<map_key_to_list_of_occurances>(new_map);
        swap(list_of_pairs, new_list);
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
    }

    keyed_queue(const keyed_queue & old_queue) {
        keyed_queue new_queue;
        new_queue.list_of_pairs = old_queue.list_of_pairs;
        new_queue.map_of_iterators = old_queue.map_of_iterators;
        //poprawka z shallow_copy_enable na old_queue.shallow_copy_enable
        if (old_queue.shallow_copy_enable == false) {
            //DODANE:
            new_queue.change_set = old_queue.change_set;
            new_queue.full_copy(*this);
        }

        list_of_pairs = new_queue.list_of_pairs;
        map_of_iterators = new_queue.map_of_iterators;
        shallow_copy_enable = true;
    }

//    keyed_queue(keyed_queue &&) noexcept = default;

    keyed_queue& operator=(keyed_queue  old_queue) {
        keyed_queue new_queue = keyed_queue(old_queue);

        list_of_pairs = new_queue.list_of_pairs;
        map_of_iterators = new_queue.map_of_iterators;
        shallow_copy_enable = true;
        return *this;
    };

    void push(K const &k, V const &v) {

        std::shared_ptr <K> k_ptr = std::make_shared<K>(k);//
        auto map_ptr = map_of_iterators;
        auto list_ptr = list_of_pairs;
        bool wasnt_unique = false;

        //When there is another on queue that's using the same resource as this.
        //We have to fully copy its resources.
        if (map_of_iterators.use_count() > 2) {
            wasnt_unique = true;
            keyed_queue<K, V> new_queue(*this);
            new_queue.full_copy(*this);//TODO moze byc podwojna kopia
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
            found = ret.first;

        }

        if (wasnt_unique) {
            map_of_iterators = map_ptr;
            list_of_pairs = list_ptr;
        }

        list_of_pairs->splice(list_of_pairs->end(), singleton);
        found->second.splice((found->second).end(), singletonIt);
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
        //Like in push we have to fully copy queue's resources.
        if (map_of_iterators.use_count() > 2) {
            wasnt_unique = true;
            keyed_queue<K, V> new_queue(*this);
            new_queue.full_copy(*this);//TODO moze byc podwojna kopia
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
    }

    void move_to_back(K const &k) {
        auto k_ptr = std::make_shared<K>(k);
        auto found = map_of_iterators->find(k_ptr);

        if (found == map_of_iterators->end()) {
            throw lookup_error();
        }

        std::list <pairKV> new_list_of_pairs;
        std::list <itKV> new_list_of_iterators;
        //Here we gather elements that we have to move back.
        for (auto i : found->second) {
            new_list_of_pairs.push_back(*i);
        }
        //The same with iterators.
        for (auto i = new_list_of_pairs.begin(); i != new_list_of_pairs.end(); ++i) {
            new_list_of_iterators.push_back(i);
        }
        //Like in push, pop we have to fully copy resources
        //when this queue shares them.
        if (map_of_iterators.unique() == false) {
            keyed_queue<K, V> new_queue(*this);
            new_queue.full_copy(*this);
            //This is last operation that may throw an exception.
            //It is also the first one that modifies anythng.
            found = new_queue.map_of_iterators->find(k_ptr);
            map_of_iterators = new_queue.map_of_iterators;
            list_of_pairs = new_queue.list_of_pairs;
        }
        //Erasing elements from list and map.
        while (found->second.empty() == false) {
            // std::cout<<"usuwam "<< *found->second.front()->first <<"  "<<*found->second.front()->second<<"\n";
            list_of_pairs->erase(found->second.front());
            found->second.pop_front();
        }
        //Adding them in right order.
        list_of_pairs->splice(list_of_pairs->end(), new_list_of_pairs);
        swap(found->second, new_list_of_iterators);
    }

    std::pair<K const &, V &> front() {
        if (empty()) {
            throw lookup_error();
        }

        change_single(list_of_pairs->begin());

        K const &key = *list_of_pairs->front().first;//Te linie mog rzucac TODO
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
        return list_of_pairs->size();
    }

    bool empty() const {
        return size() == 0;
    }

    void clear() {
        *this = keyed_queue<K,V>();
    }

    size_t count(K const &k) const {
        auto found = map_of_iterators->find(std::make_shared<K>(k));
        if (found == map_of_iterators->end()) {
            return 0;
        }
        return found->second.size();
    }
};

#endif //JNP_ZADANIE5_KEYED_QUEUE1_H