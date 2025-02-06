#include <iostream>

#include "caches.hpp"

int slow_get_page(int key) {
  return key;
}

int main() {
  std::size_t sz;
  std::cin >> sz;
  caches::cache_t<int> cache{sz};

  std::size_t hits = 0;
  std::size_t count = 0;
  int key;
  while(std::cin >> key){
    auto hit= cache.lookup_update(key, slow_get_page);
    if(hit) std::cout << "#" << count << " Hit " << key << "\n";
    hits+=hit;
    count++;
  }
  std::cout << "All: " << count << "\n";
  std::cout << "Hits: " << hits << "\n";
  std::cout << "Perecentage: " <<  (float)hits/count*100 << "%\n";
  return 0;
}