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

    //Attribute remembers if queue gave any reference.
    bool shallow_copy_enable = true;
    //Set of 'given' values.
    std::set<std::shared_ptr<V>> change_set;

    //{shard pointer on key, shared pointer on value}
    using pairKV = std::pair <std::shared_ptr<K>, std::shared_ptr<V>>;
    using itKV = typename std::list<pairKV>::iterator;
    std::shared_ptr <std::list<pairKV>> list_of_pairs;

    struct cmp_map {
        bool operator()(const std::shared_ptr <K> k1, const std::shared_ptr <K> k2) const {
            return *k1 < *k2;
        }
    };

    using list_of_iterators = std::list<itKV>;
    //Map assigning list of iterators itKV for every shared pointer on key.
    using map_key_to_list_of_occurances  = std::map <std::shared_ptr<K>, list_of_iterators, cmp_map>;
    std::shared_ptr <map_key_to_list_of_occurances> map_of_iterators;

    //DODANA: funkcja pomocnicza do oddawania referencji
    //rozumiem, ze w metodach gdie jest V const & nie ma sensu tego robić
    void change_single(itKV it) {
      std::cout << "CHANGE:\n";
      shallow_copy_enable= false;
      // std::cout << "change single: " << it->second.use_count() << "\n";
      if(list_of_pairs.unique() == false) {
          //zmiana tego:
          // full_copy();

          //na:
          //z takich powodów, że teraz full_copy by wyczyściło nam change_set
          auto new_queue = keyed_queue(*this);
          this->map_of_iterators = new_queue.map_of_iterators;
          this->list_of_pairs = new_queue.list_of_pairs;
      }
      it->second = std::make_shared<K>(*it->second);
      change_set.insert(it->second);
    }

    //Function that copies resources of queue, so it doesn't share its resources.
    void full_copy() {
      std::cout << "PELNA KOPIA\n";
        std::list < pairKV > empty_list;
        std::shared_ptr <std::list<pairKV>>
            new_list = std::make_shared < std::list < pairKV >>(empty_list);

        //Copying list_of_pairs
        for(auto it = list_of_pairs->begin();it!=list_of_pairs->end();++it){
            //new_list->push_back(*it);

            //DODANE:
            auto found = change_set.find(it->second);
            if(found != change_set.end()) {
                // std::cout << it->second << " ";
                new_list->push_back({it->first, std::make_shared<K>(*it->second)});
                // std::cout << (--new_list->end())->second << " " << *it->second << "\n";
            }
            else
              new_list->push_back(*it);
        }

        //DODANE
        change_set.clear();

        map_key_to_list_of_occurances new_map;

        //Copying map shared_ptr<K> -> list_of_iterators
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

  void print_queue() {
      auto i = list_of_pairs->begin();
      while (i != list_of_pairs->end()) {
          std::cout << *i->first << " " << *i->second << " | ";
          ++i;
      }
      std::cout << "\n";
  }

    void pom() {
      auto it = --list_of_pairs->end();
      std::cout << "POM: ";
      std::cout << it->second << " " << *it->second << "\n";
    }

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


        //TODO licznik
        //poprawka z shallow_copy_enable na old_queue.shallow_copy_enable
        if (old_queue.shallow_copy_enable == false) {
            //DODANE:
            new_queue.change_set = old_queue.change_set;
            new_queue.full_copy();
        }
        list_of_pairs = new_queue.list_of_pairs;
        map_of_iterators = new_queue.map_of_iterators;
        shallow_copy_enable = true;
    }

    keyed_queue(keyed_queue &&) = default;

    void push(K const &k, V const &v) {
        std::cout << "PUSH: " << k << " " << v << "\n";

        std::shared_ptr <K> k_ptr = std::make_shared<K>(k);//
        auto map_ptr = map_of_iterators;
        auto list_ptr = list_of_pairs;
        bool wasnt_unique = false;

        //When there is another on queue that's using the same resource as this.
        //We have to fully copy its resources.
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

        print_queue();
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

        //Like in push we have to fully copy queue's resources.
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

        print_queue();
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
            new_queue.full_copy();
            found = new_queue.map_of_iterators->find(k_ptr);
            map_of_iterators = new_queue.map_of_iterators;
            list_of_pairs = new_queue.list_of_pairs;

            found = map_of_iterators->find(k_ptr);//TODO usun
        }
//Od teraz no-throw
        //Erasing elements from list and map.
        while (found->second.empty() == false) {
            // std::cout<<"usuwam "<< *found->second.front()->first <<"  "<<*found->second.front()->second<<"\n";
            list_of_pairs->erase(found->second.front());
            found->second.pop_front();
        }
        //Adding them in right order.
        list_of_pairs->splice(list_of_pairs->end(), new_list_of_pairs);
        swap(found->second, new_list_of_iterators);

        print_queue();
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
