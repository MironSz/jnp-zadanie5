#ifndef JNP_ZADANIE5_KEYED_QUEUE_H
#define JNP_ZADANIE5_KEYED_QUEUE_H

template<class K, class V>
class keyed_queue {
private:
    using pairKV = std::pair<K, V>;
    using itKV = std::list<pairKV>::iterator;
    using list_of_itarator = list<itKV>;

    std::list <pairKV> list_of_pairs;
    std::map <key, list_of_itarator> map_key_to_list_of_occurances;

public:
    keyed_queue() {

    }

    keyed_queue(keyed_queue const &old_queue) {

    }

    keyed_queue(keyed_queue &&) {

    }

    void push(K const &k, V const &v) {
        list_of_pairs.push_back(pairKV(k, v));//Nie powinniśmy kopiować?

        if (map_key_to_list_of_occurances.contains(k) == false) {
            map_key_to_list_of_occurances.insert(k, list_of_iterators{});
        }

        map_key_to_list_of_occurances[key].push_back(list_of_pairs.end()--);
    }

    void pop() {
        if (list_of_pairs.isEmpty() == false) {

        }
        pairKV p = list_of_pairs.front();

        map_key_to_list_of_occurances[p.first].
                erase(map_key_to_list_of_occurances[p.first].begin());

        if (map_key_to_list_of_occurances[p.first].isEmpty()) {
            map_key_to_list_of_occurances.remove(p.first);
        }
    }

    void pop(K const &k) {
        if (map_key_to_list_of_occurances.contains(k) == false) {

        }

        itKV it = map_key_to_list_of_occurances[k].front();

        list_of_pairs.erase(it);

        map_key_to_list_of_occurances[k].
                erase(map_key_to_list_of_occurances[k].begin());

        if (map_key_to_list_of_occurances[k].isEmpty()) {
            map_key_to_list_of_occurances.remove(k);
        }
    }

    void move_to_back(K const &k) {
        if (map_key_to_list_of_occurances.contains(k) == false) {

        }

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

    }

    bool empty() const {

    }

    void clear() {

    }

    size_t count(K const &) const {

    }

//    - konstruktor bezparametrowy i kopiujący
//    - operator++ prefiksowy
//    - operator== i operator!=
//    - operator*
};

#endif //JNP_ZADANIE5_KEYED_QUEUE_H
