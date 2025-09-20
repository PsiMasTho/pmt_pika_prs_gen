// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/parser/builder/program.hpp"
#endif
// clang-format on

#include "pmt/base/overloaded.hpp"

namespace pmt::parser::builder {

auto Program::LitSeqTypeIndirectEq::operator()(auto const& lhs_, auto const& rhs_) const -> bool {
 auto const fetcher = pmt::base::Overloaded{[&](ArgType lit_seq_id_) -> LitSeqType const& { return _table[lit_seq_id_]; },
                                            [](LitSeqType const& lit_seq_) -> LitSeqType const& {
                                             return lit_seq_;
                                            }};

 return fetcher(lhs_) == fetcher(rhs_);
}

}  // namespace pmt::parser::builder