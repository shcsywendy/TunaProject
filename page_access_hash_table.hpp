#ifndef PAGE_ACCESS_HASH_TABLE_HPP
#define PAGE_ACCESS_HASH_TABLE_HPP

#include <unordered_map>

// Define a struct to represent page access information
struct PageAccess {
    int pageAddress;
    int numAccesses;
};

// Define a hash function for PageAccess
struct PageAccessHash {
    std::size_t operator()(const PageAccess& pa) const;
};

// Define a custom equality operator for PageAccess
bool operator==(const PageAccess& lhs, const PageAccess& rhs);

#endif // PAGE_ACCESS_HASH_TABLE_HPP
