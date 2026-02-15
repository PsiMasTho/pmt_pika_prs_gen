#pragma once

#include "pmt/pika/rt/primitives.hpp"

#include <queue>
#include <vector>

namespace pmt::pika::rt {
class ClauseBase;

class ClauseQueueItem {
public:
 ClauseBase const* _clause;
 IdType _priority;
};

class ClauseQueueCmp {
public:
 auto operator()(ClauseQueueItem const& lhs_, ClauseQueueItem const& rhs_) const -> bool {
  return lhs_._priority < rhs_._priority;
 }
};

using ClauseQueue = std::priority_queue<ClauseQueueItem, std::vector<ClauseQueueItem>, ClauseQueueCmp>;

}  // namespace pmt::pika::rt