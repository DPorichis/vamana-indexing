#include "graph.h"

// Method to add a distance to cache
void distcache::putDistance(Node point_a, Node point_b, double distance) 
{
    pair<Node, Node> key = {point_a, point_b};
    
    // If we have this distanced stored
    if (storage.find(key) != storage.end()) 
    {
        lru_list.erase(storage[key].second); // Remove its instance from the LRU list
    } 

    // If we dont have it, and we are out of room
    if (lru_list.size() >= capacity) {
        storage.erase(lru_list.front().first); // Delete most recently used
        lru_list.pop_front(); // Remove from LRU
    }

    lru_list.push_back({key, distance}); // Place the new pair at the end of the LRU list
    storage[key] = {distance, std::prev(lru_list.end())}; // Store its value and its LRU iterator for faster accessing

}

// Method to retrieve the distance from the cache
double distcache::getDistance(Node point_a, Node point_b) {
    std::pair<Node, Node> key = {point_a, point_b};
    
    auto it = storage.find(key);
    if (it == storage.end()) {
        // Try the mirror
        key = {point_b, point_a};
        it = storage.find(key);

        if (it == storage.end())
            return -1; // Not found
    }
    
    // Entry found, updating lru list
    lru_list.erase(it->second.second);
    lru_list.push_back({key, it->second.first});
    storage[key] = {it->second.first, std::prev(lru_list.end())}; // Update its lru iterator
    return it->second.first;
}