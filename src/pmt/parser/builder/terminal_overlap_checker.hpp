#pragma once

#include "pmt/parser/grammar/grammar_data.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"

#include <unordered_set>

namespace pmt::parser::builder {

class TerminalOverlapChecker {
 public:
 // -$ Types / Constants $-
 public:
  class Args {
   public:
    pmt::util::sm::ct::StateMachine const& _state_machine;
    pmt::parser::grammar::GrammarData const& _grammar_data;
    GenericAst const& _ast;
    pmt::util::sm::StateNrType _state_nr_from = pmt::util::sm::StateNrStart;
  };

 // --$ Other $--
 static auto find_overlaps(Args args_) -> std::unordered_set<pmt::base::IntervalSet<pmt::util::sm::AcceptsIndexType>>;
};

}