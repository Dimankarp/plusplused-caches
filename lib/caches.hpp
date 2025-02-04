#pragma once

#include <cstddef>
#include <list>
#include <unordered_map>

/* NOTICE!: This module is intentionally written in C style as part
of C++ studying procees. */

namespace caches {

template <typename T, typename KeyT = int>
struct cache_t {
  std::size_t sz_;
  std::list<std::pair<KeyT, T>> cache_;

  using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;
  std::unordered_map<KeyT, ListIt> map_;

  explicit cache_t(std::size_t sz) : sz_{sz} {
  }

  bool is_full() const {
    return cache_.size() == sz_;
  }

  template <typename F>
  bool lookup_update(KeyT key, F slow_get_page) {
    auto hit = map_.find(key);
    if (hit == map_.end()) {
      if (is_full()) {
        map_.erase(cache_.back().first);
        cache_.pop_back();
      }
      cache_.emplace_front(key, slow_get_page(key));
      map_.emplace(key, cache_.begin());
      return false;
    }

    auto itemit = hit->second;
    cache_.splice(cache_.begin(), cache_, itemit);
    return true;
  }
};

}  // namespace caches