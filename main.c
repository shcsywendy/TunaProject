#include <stdio.h>
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
        printf("Page %d was accessed %d times.\n", searchPage.pageAddress, it->second);
    } else {
        printf("Page %d not found in the hash table.\n", searchPage.pageAddress);
    }

    return 0;
}
