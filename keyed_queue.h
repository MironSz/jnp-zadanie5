#ifndef JNP_ZADANIE5_KEYED_QUEUE_H
#define JNP_ZADANIE5_KEYED_QUEUE_H

template<class K, class V>
class keyed_queue {
private:
    using pairKV = std::pair<K, V>;
    using list_of_itarator = list<std::list<pairKV>::iterator>;
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

    }

    void pop(K const &) {

    }

    void move_to_back(K const &k) {

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
