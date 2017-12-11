#ifndef JNP_ZADANIE5_KEYED_QUEUE_H
#define JNP_ZADANIE5_KEYED_QUEUE_H

#include <stdlib.h>
#include <utility>
#include <list>
#include <map>

class lookup_error : public std::exception {
  virtual const char* what() const throw()
  {
    return "Lookup error";
  }
};

template<class K, class V>
class keyed_queue {
private:
    using pairKV = std::pair<K, V>;
    using itKV = typename std::list<pairKV>::iterator;
    using list_of_itarator = std::list<itKV>;

    std::list <pairKV> list_of_pairs;
    std::map <K, list_of_itarator> map_key_to_list_of_occurances;
public:
    using k_iterator = typename std::map <K, list_of_itarator>::const_iterator;

    k_iterator k_begin(){
        return map_key_to_list_of_occurances.begin();
    }

    k_iterator k_end(){
        return map_key_to_list_of_occurances.end();//Chyba tak, ew --end()
    }

    keyed_queue() {

    }

    keyed_queue(keyed_queue const &old_queue) {

    }

    keyed_queue(keyed_queue &&) {

    }

    void push(K const &k, V const &v) {
        list_of_pairs.push_back(pairKV(k, v));//Nie powinniśmy kopiować?

        if (map_key_to_list_of_occurances.find(k) == map_key_to_list_of_occurances.end())
            map_key_to_list_of_occurances.insert(std::make_pair(k, list_of_itarator{}));

        map_key_to_list_of_occurances[k].push_back(--list_of_pairs.end());
    }

    void pop() {
        if (empty())
            throw lookup_error();
        pairKV p = list_of_pairs.front();

        map_key_to_list_of_occurances[p.first].
                erase(map_key_to_list_of_occurances[p.first].begin());

        if (map_key_to_list_of_occurances[p.first].empty())
            map_key_to_list_of_occurances.erase(p.first);

        list_of_pairs.pop_front();
    }

    void pop(K const &k) {
        if (map_key_to_list_of_occurances.find(k) == map_key_to_list_of_occurances.end())
            throw lookup_error();

        itKV it = map_key_to_list_of_occurances[k].front();

        list_of_pairs.erase(it);

        map_key_to_list_of_occurances[k].
                erase(map_key_to_list_of_occurances[k].begin());

        if (map_key_to_list_of_occurances[k].empty())
            map_key_to_list_of_occurances.erase(k);
    }

    void move_to_back(K const &k) {
        if (map_key_to_list_of_occurances.find(k) == map_key_to_list_of_occurances.end())
            throw lookup_error();

        list_of_itarator l = map_key_to_list_of_occurances[k];
        int number_of_elements_to_move = l.size();

        for (int i = 0; i < number_of_elements_to_move; i++) {
            itKV it = l.front();
            list_of_pairs.push_back(*it);
            list_of_pairs.erase(it);
            l.pop_front();
            l.push_back(list_of_pairs.end()--);
        }

        map_key_to_list_of_occurances[k] = l;
    }

    std::pair<K const &, V &> front() {

    };

    std::pair<K const &, V &> back() {

    };

    std::pair<K const &, V const &> front() const {

    };

    std::pair<K const &, V const &> back() const {

    };

    std::pair<K const &, V &> first(K const &key) {

    };

    std::pair<K const &, V &> last(K const &key) {

    };

    std::pair<K const &, V const &> first(K const &key) const {

    };

    std::pair<K const &, V const &> last(K const &key) const {

    };

    size_t size() const {
        return list_of_pairs.size();
    }

    bool empty() const {
        return size() == 0;
    }

    void clear() {

    }

    size_t count(K const &k) const {
        if(map_key_to_list_of_occurances.find(k) == map_key_to_list_of_occurances.end())
            throw lookup_error();
        return map_key_to_list_of_occurances[k].size();
    }

//    - konstruktor bezparametrowy i kopiujący
//    - operator++ prefiksowy
//    - operator== i operator!=
//    - operator*
};

#endif //JNP_ZADANIE5_KEYED_QUEUE_H
