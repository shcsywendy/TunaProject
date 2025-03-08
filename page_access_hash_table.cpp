#include "page_access_hash_table.hpp"

// Define the hash function for PageAccess
std::size_t PageAccessHash::operator()(const PageAccess& pa) const {
    return std::hash<int>{}(pa.pageAddress);
}

// Define the custom equality operator for PageAccess
bool operator==(const PageAccess& lhs, const PageAccess& rhs) {
    return lhs.pageAddress == rhs.pageAddress;
}
