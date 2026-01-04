#pragma once

#include "pmt/fw_decl.hpp"

#include <queue>
#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::parser, ClauseBase);

namespace pmt::parser::rt {

class ClauseQueueItem {
public:
 ClauseBase const* _clause;
 int _priority;
};

class ClauseQueueComparator {
public:
 auto operator()(ClauseQueueItem const& lhs_, ClauseQueueItem const& rhs_) const -> bool {
  return lhs_._priority < rhs_._priority;
 }
};

using ClauseQueue = std::priority_queue<ClauseQueueItem, std::vector<ClauseQueueItem>, ClauseQueueComparator>;

}  // namespace pmt::parser::rt