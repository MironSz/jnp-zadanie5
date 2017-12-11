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

    class k_iterator {
    private:
        typename std::map<K, list_of_itarator>::const_iterator i;

    public:
        k_iterator(typename std::map<K, list_of_itarator>::const_iterator it) : i(it) {};

        k_iterator& operator++() {
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
    k_iterator k_begin(){
        return k_iterator(map_key_to_list_of_occurances.begin());
    }

    k_iterator k_end(){
        return k_iterator(map_key_to_list_of_occurances.end());//Chyba tak, ew --end()
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
      auto found = map_key_to_list_of_occurances.find(k);
      if (found == map_key_to_list_of_occurances.end())
          throw lookup_error();

        itKV it = found->second.front();

        list_of_pairs.erase(it);

        found->second.erase(found->second.begin());

        if (found->second.empty())
            map_key_to_list_of_occurances.erase(k);
    }

    void move_to_back(K const &k) {
      auto found = map_key_to_list_of_occurances.find(k);
      if (found == map_key_to_list_of_occurances.end())
          throw lookup_error();

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
        if (empty())
            throw lookup_error();
        K const& key = list_of_pairs.front().first;
        V& val = list_of_pairs.front().second;
        return {key, val};
    };

    std::pair<K const &, V &> back() {
      if (empty())
          throw lookup_error();
      K const& key = list_of_pairs.back().first;
      V& val = list_of_pairs.back().second;
      return {key, val};
    };

    std::pair<K const &, V const &> front() const {
      if (empty())
          throw lookup_error();
      K const& key = list_of_pairs.front().first;
      V const& val = list_of_pairs.front().second;
      return {key, val};    };

    std::pair<K const &, V const &> back() const {
      if (empty())
          throw lookup_error();
      K const& key = list_of_pairs.back().first;
      V const& val = list_of_pairs.back().second;
      return {key, val};
    };

    std::pair<K const &, V &> first(K const &key) {
      auto found = map_key_to_list_of_occurances.find(key);
      if (found == map_key_to_list_of_occurances.end())
          throw lookup_error();
      V& val = found->second.front()->second;
      return {key, val};
    };

    std::pair<K const &, V &> last(K const &key) {
      auto found = map_key_to_list_of_occurances.find(key);
      if (found == map_key_to_list_of_occurances.end())
          throw lookup_error();
      V& val = found->second.back()->second;
      return {key, val};
    };

    std::pair<K const &, V const &> first(K const &key) const {
      auto found = map_key_to_list_of_occurances.find(key);
      if (found == map_key_to_list_of_occurances.end())
          throw lookup_error();
      V const& val = found->second.front()->second;
      return {key, val};
    };

    std::pair<K const &, V const &> last(K const &key) const {
      auto found = map_key_to_list_of_occurances.find(key);
      if (found == map_key_to_list_of_occurances.end())
          throw lookup_error();
      V const& val = found->second.back()->second;
      return {key, val};
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

};

#endif //JNP_ZADANIE5_KEYED_QUEUE_H
