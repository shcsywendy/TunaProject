#include <iostream>
#include <unordered_map>
#include "page_access_hash_table.hpp"

int main() {
    // Create an unordered_map using PageAccess as the key type
    std::unordered_map<PageAccess, int, PageAccessHash> pageAccessCount;

    // Insert some data into the hash table
    PageAccess page1 = {100, 5};
    PageAccess page2 = {200, 3};
    PageAccess page3 = {300, 7};

    pageAccessCount[page1] = 5;
    pageAccessCount[page2] = 3;
    pageAccessCount[page3] = 7;

    // Retrieve and print the number of accesses for a specific page address
    int searchPageAddress = 200;
    PageAccess searchPage = {searchPageAddress, 0};

    auto it = pageAccessCount.find(searchPage);
    if (it != pageAccessCount.end()) {
        std::cout << "Page " << searchPage.pageAddress << " was accessed " << it->second << " times." << std::endl;
    } else {
        std::cout << "Page " << searchPage.pageAddress << " not found in the hash table." << std::endl;
    }

    return 0;
}
