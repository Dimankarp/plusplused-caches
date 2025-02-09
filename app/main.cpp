#include <iostream>

#include "caches.hpp"
#include "perfect.hpp"

int slow_get_page(int key) {
  return key;
}

int main() {
  std::size_t sz;
  std::cin >> sz;
  caches::cache_t<int> cache{sz};

  std::size_t hits = 0;
  std::size_t count = 0;
  std::vector<std::size_t> requests{};
  int key;
  while (std::cin >> key) {
    requests.push_back(key);
    auto hit = cache.lookup_update(key, slow_get_page);
    if (hit)
      std::cout << "#" << count << " Hit " << key << "\n";
    hits += hit;
    count++;
  }
  std::cout << "All: " << count << "\n";
  std::cout << "Hits: " << hits << "\n";
  std::cout << "Perecentage: " << (float)hits / count * 100 << "%\n";

  auto perfect_hits = caches::perfect_get_hits<size_t>(sz, requests.begin(), requests.end());
  std::cout << "Perfect cache hits: " << perfect_hits << "\n";
  std::cout << "Perecentage of perfect: " << (float)hits / perfect_hits * 100 << "%\n";
  return 0;
}