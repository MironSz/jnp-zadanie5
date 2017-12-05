#ifndef JNP_ZADANIE5_KEYED_QUEUE_H
#define JNP_ZADANIE5_KEYED_QUEUE_H

template <class K, class V> class keyed_queue {
public:
    keyed_queue(){

    }

    keyed_queue(keyed_queue const & old_queue){

    }

    keyed_queue(keyed_queue &&){

    }

    void push(K const &k, V const &v){

    }

    void pop(){

    }

    void move_to_back(K const &k){

    }

    std::pair<K const &, V &> front(){

    };

    std::pair<K const &, V &> back(){

    };

    std::pair<K const &, V const &> front() const{

    };

    std::pair<K const &, V const &> back() const{

    };

    std::pair<K const &, V &> first(K const &key){

    };

    std::pair<K const &, V &> last(K const &key){

    };

    std::pair<K const &, V const &> first(K const &key) const{

    };

    std::pair<K const &, V const &> last(K const &key) const{

    };

    size_t size() const{

    }

    bool empty() const{

    }

    void clear(){

    }

    size_t count(K const &) const{

    }

//    - konstruktor bezparametrowy i kopiujący
//    - operator++ prefiksowy
//    - operator== i operator!=
//    - operator*
};

#endif //JNP_ZADANIE5_KEYED_QUEUE_H
