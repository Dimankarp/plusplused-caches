#pragma once

#include <algorithm>
#include <cstddef>
#include <list>
#include <unordered_map>
#include <variant>

/* NOTICE!: This module is intentionally written in C style as part
of my C++ study. */

/* Implementation of ARC policy from
 * https://www.usenix.org/legacy/events/fast03/tech/full_papers/megiddo/megiddo.pdf */
namespace caches {
using std::list, std::unordered_map, std::size_t, std::pair, std::variant;

template <typename T, typename KeyT = int>
struct cache_t {
  size_t sz_;
  /* How often is entry used (recent = 1 time, often >= 2) */
  enum class entry_status : char { GHOST_RECENT, RECENT, GHOST_OFTEN, OFTEN };

  list<KeyT> ghost_recent_;
  list<pair<KeyT, T>> recent_;
  list<KeyT> ghost_often_;
  list<pair<KeyT, T>> often_;

  /* Adaptating size of recent lists */
  size_t p = 0;

  using ListIt = typename list<pair<KeyT, T>>::iterator;
  using GhostIt = typename list<KeyT>::iterator;

  unordered_map<KeyT, pair<entry_status, variant<ListIt, GhostIt>>> map_;

  explicit cache_t(size_t sz) : sz_{sz} {
  }

  size_t size() const {
    return ghost_recent_.size() + recent_.size() + ghost_often_.size() + often_.size();
  }

  bool is_full() const {
    return size() == sz_;
  }

  template <typename F>
  bool lookup_update(KeyT key, F slow_get_page) {
    auto hit = map_.find(key);
    if (hit == map_.end()) {
      major_cache_miss_(key, slow_get_page);
      return false;
    }

    auto itemit = hit->second;
    switch (itemit.first) {
      case entry_status::RECENT: {
        auto it = std::get<ListIt>(itemit.second);
        often_.splice(often_.end(), recent_, it);
        hit->second.first = entry_status::OFTEN;
        return true;
      }
      case entry_status::OFTEN: {
        auto it = std::get<ListIt>(itemit.second);
        often_.splice(often_.end(), often_, it);
        return true;
      }
      case entry_status::GHOST_RECENT: {
        enlarge_p_();
        replace_(false);

        T&& item = slow_get_page(key);

        auto it = std::get<GhostIt>(itemit.second);
        ghost_recent_.erase(it);

        often_.emplace_back(key, item);
        hit->second = pair{entry_status::OFTEN, variant<ListIt, GhostIt>{std::prev(often_.end())}};
        return false;
      }
      case entry_status::GHOST_OFTEN: {
        shrink_p_();
        replace_(true);

        T&& item = slow_get_page(key);

        auto it = std::get<GhostIt>(itemit.second);
        ghost_often_.erase(it);

        often_.emplace_back(key, item);
        hit->second = pair{entry_status::OFTEN, variant<ListIt, GhostIt>{std::prev(often_.end())}};
        return false;
      }
    }
    return false;
  }

  void replace_(bool is_ghost_often) {
    auto rsz = recent_.size();
    if (rsz != 0 && (rsz > p || (is_ghost_often && rsz == p))) {
      ghost_slide_(recent_, ghost_recent_);
    } else {
      ghost_slide_(often_, ghost_often_);
    }
  }
  void ghost_slide_(list<pair<KeyT, T>>& loaded, list<KeyT>& ghost) {
    auto lru = loaded.back().first;
    loaded.pop_back();
    ghost.push_front(lru);
    map_.find(lru)->second.second = variant<ListIt, GhostIt>{ghost.begin()};
  }

  void enlarge_p_() {
    auto grsz = ghost_recent_.size();
    auto gosz = ghost_often_.size();
    size_t delta = (grsz > gosz) ? 1 : gosz / grsz;
    p = std::min(p + delta, sz_);
  }
  void shrink_p_() {
    auto grsz = ghost_recent_.size();
    auto gosz = ghost_often_.size();
    size_t delta = (gosz > grsz) ? 1 : grsz / gosz;
    p = std::max(p - delta, (size_t)0);
  }
  template <typename F>
  void major_cache_miss_(KeyT key, F slow_get_page) {
    auto full_recent_sz = recent_.size() + ghost_recent_.size();
    if (full_recent_sz == sz_) {
      if (recent_.size() < sz_) {
        remove_lru_(ghost_recent_);
        replace_(false);
      } else {
        remove_lru_(recent_);
      }
    } else {
      if (size() > sz_) {
        if (size() == 2 * sz_) {
          remove_lru_(ghost_often_);
        }
        replace_(false);
      }
    }
    recent_.emplace_back(key, slow_get_page(key));
    map_.emplace(
        key, pair(entry_status::RECENT, variant<ListIt, GhostIt>(std::prev(recent_.end())))
    );
  }

  void remove_lru_(list<pair<KeyT, T>> list) {
    auto lru = list.front().second;
    map_.erase(lru);
    list.pop_front();
  }

  void remove_lru_(list<KeyT> list) {
    auto lru = list.front();
    map_.erase(lru);
    list.pop_front();
  }
};

}  // namespace caches