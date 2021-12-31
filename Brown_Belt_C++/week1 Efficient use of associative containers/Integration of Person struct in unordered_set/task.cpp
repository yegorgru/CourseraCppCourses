#include <limits>
#include <random>
#include <unordered_set>

using namespace std;

struct Address {
    string city, street;
    int building;

    bool operator==(const Address& other) const {
        return building == other.building &&
            city == other.city &&
            street == other.street;
    }
};

struct Person {
    string name;
    int height;
    double weight;
    Address address;

    bool operator==(const Person& other) const {
        return height == other.height &&
            weight == other.weight &&
            name == other.name &&
            address == other.address;
    }
};

struct AddressHasher {
    size_t operator() (const Address& address) const {
        size_t n = 3'000'000;
        return s_hasher(address.city) * n * n + s_hasher(address.street) * n + address.building;
    }
    
    hash<string>s_hasher;
};

struct PersonHasher {
    size_t operator() (const Person& person) const {
        size_t n = 3'123'000;
        return s_hasher(person.name) * n * n * n + a_hasher(person.address) * n * n + person.height * n + person.weight;
    }

    hash<string>s_hasher;
    AddressHasher a_hasher;
};