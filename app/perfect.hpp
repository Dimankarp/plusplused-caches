#pragma once

#include <climits>
#include <cstddef>
#include <functional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace caches {

using std::size_t;

template <typename KeyT, typename It>
size_t perfect_get_hits(size_t sz, It start, It end) {
  struct indexed_key {
    KeyT key;
    size_t index;

    bool operator<(const indexed_key& y) {
      return index < y.index;
    }
  };
  std::priority_queue<indexed_key, std::vector<indexed_key>, std::less<>> queue_{};
  std::unordered_set<KeyT> hash_{};
  std::unordered_map<KeyT, std::queue<size_t>> idxs_{};
  size_t index = 0;
  for (auto it = start; it != end; it++) {
    idxs_[*it].push(index);
  }
  size_t hits = 0;
  for (auto it = start; it != end; it++) {
    auto key = *it;
    idxs_[key].pop();
    if (hash_.find(key) != hash_.end()) {
      hits++;
    }

    if (hash_.size() == sz) {
      hash_.erase(queue_.top().key);
      queue_.pop();
    }
    auto next = !idxs_[key].empty() ? idxs_[key].front() : INT_MAX;
    indexed_key indexed = {key, next};
    queue_.push(indexed);
    hash_.insert(key);
  }
  return hits;
};

}  // namespace caches