#include <iostream>
#include <list>
#include <memory>
#include <unordered_map>


template <typename KeyType, typename ValueType>
class Cache {
    struct Node {
        Node(const KeyType& k, std::shared_ptr<ValueType> v) : key(k), value(v) {};
        const KeyType key;
        std::shared_ptr<ValueType> value;
    };
    const std::size_t size;
    std::list<Node> list;
    std::unordered_map<KeyType, typename std::list<Node>::iterator> map;
    bool key_exists(const KeyType& key) {
        auto existing = map.find(key);
        if (existing == map.end()) {
            return false;
        }
        list.splice(list.begin(), list, existing->second);
        return true;
    }
public:
    Cache (std::size_t size) : size(size) {}
    std::weak_ptr<ValueType> get(const KeyType& key) {
        return key_exists(key) ? list.front().value : nullptr;
    }
    void insert (const KeyType& key, const ValueType& value) {
        if (key_exists(key)) {
            list.front().value = std::make_shared<ValueType> (value);
        }
        else {
            list.emplace_front(key, std::make_shared<ValueType> (value));
            map.emplace(key, list.begin());
            if (list.size() > size) {
                map.erase(list.back().key);
                list.pop_back();
            }
        }
    }
};


template <typename T>
void print_wp(const std::weak_ptr<T>& x) {
    if (auto sp = x.lock()) {
        std::cout << *sp << std::endl;
    }
    else {
        std::cout << "weak_ptr expired" << std::endl;
    }
}


int main() {
    Cache<int, std::string> cache {2};
    cache.insert(1, "test1");
    cache.insert(2, "test2");
    cache.insert(1, "test1");
    cache.insert(3, "test3");
    print_wp(cache.get(1));
    print_wp(cache.get(2));
    print_wp(cache.get(3));
    return 0;
}
