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
    using pairKV = std::pair<std::shared_ptr<K>, std::shared_ptr<V>>;
    using itKV = typename std::list<pairKV>::iterator;
    using list_of_itarator = std::list<itKV>;
    using map_key_to_list_of_occurances  = std::map <std::shared_ptr<K>, list_of_itarator>;
    bool shallow_copy_enable;
    std::shared_ptr<std::list<pairKV>>  list_of_pairs;

    std::shared_ptr<map_key_to_list_of_occurances> map_of_iterators;

    void full_copy(){
        auto new_list = new std::list<pairKV>(*list_of_pairs.get());

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
            return i->first;
        }
    };

public:
    k_iterator k_begin() {
        return k_iterator(map_of_iterators.begin());
    }

    k_iterator k_end() {
        return k_iterator(map_of_iterators.end());
    }

    keyed_queue() {
        
    }

    keyed_queue(keyed_queue const &old_queue) {

    }

    keyed_queue(keyed_queue &&) = default;

    void push(K const &k, V const &v) {
        auto found = map_of_iterators.find(k);

        if (found == map_of_iterators.end()) {
            map_of_iterators.insert(std::make_pair(k, list_of_itarator{}));
            found = map_of_iterators.find(k);
        }

        list_of_pairs.push_back(pairKV(&(found->first), v));

        found->second.push_back(--list_of_pairs.end());
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
        auto found = map_of_iterators.find(k);
        if (found == map_of_iterators.end()) {
            throw lookup_error();
        }

        itKV it = found->second.front();

        list_of_pairs.erase(it);

        found->second.erase(found->second.begin());

        if (found->second.empty()) {
            map_of_iterators.erase(k);
        }
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
        auto found = map_of_iterators.find(key);
        if (found == map_of_iterators.end()) {
            throw lookup_error();
        }
        V &val = found->second.front()->second;
        return {key, val};
    };

    std::pair<K const &, V &> last(K const &key) {
        auto found = map_of_iterators.find(key);
        if (found == map_of_iterators.end()) {
            throw lookup_error();
        }
        V &val = found->second.back()->second;
        return {key, val};
    };

    std::pair<K const &, V const &> first(K const &key) const {
        auto found = map_of_iterators.find(key);
        if (found == map_of_iterators.end()) {
            throw lookup_error();
        }
        V const &val = found->second.front()->second;
        return {key, val};
    };

    std::pair<K const &, V const &> last(K const &key) const {
        auto found = map_of_iterators.find(key);
        if (found == map_of_iterators.end()) {
            throw lookup_error();
        }
        V const &val = found->second.back()->second;
        return {key, val};
    };

    size_t size() const {
        return list_of_pairs.get();
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
