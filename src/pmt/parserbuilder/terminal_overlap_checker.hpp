#pragma once

#include "pmt/parserbuilder/grammar_data.hpp"
#include "pmt/util/smct/state_machine.hpp"

#include <unordered_set>

namespace pmt::parserbuilder {

class TerminalOverlapChecker {
 public:
 // -$ Types / Constants $-
 public:
  class Args {
   public:
    pmt::util::smct::StateMachine const& _state_machine;
    GrammarData const& _grammar_data;
    pmt::util::smrt::GenericAst const& _ast;
    pmt::util::smrt::StateNrType _state_nr_from = pmt::util::smrt::StateNrStart;
  };

 // --$ Other $--
 static auto find_overlaps(Args args_) -> std::unordered_set<pmt::base::IntervalSet<pmt::util::smrt::AcceptsIndexType>>;
};

}