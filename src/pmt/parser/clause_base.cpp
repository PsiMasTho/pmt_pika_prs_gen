#include "pmt/parser/clause_base.hpp"

namespace pmt::parser {

auto ClauseBase::get_child_id_count() const -> size_t {
 return 0;
}

auto ClauseBase::get_seed_parent_count() const -> size_t {
 return 0;
}

auto ClauseBase::can_match_zero() const -> bool {
 return false;
}

}  // namespace pmt::parser
