#include <iostream>

#include "caches.hpp"

int slow_get_page(int key) {
  return key;
}

int main() {
  caches::cache_t<int> cache{50};
  auto hit = cache.lookup_update(10, slow_get_page);
  std::cout << "Is hit: " << hit << "\n";
  hit = cache.lookup_update(10, slow_get_page);
  std::cout << "Is hit: " << hit << "\n";
  return 0;
}