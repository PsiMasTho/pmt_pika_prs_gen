#pragma once

#include "pmt/parserbuilder/grammar_data.hpp"
#include "pmt/util/smct/state_machine.hpp"

namespace pmt::parserbuilder {

class TerminalConflictChecker {
 public:
 // -$ Types / Constants $-
 public:
  class Args {
   public:
    pmt::util::smct::StateMachine& _state_machine;
    GrammarData const& _grammar_data;
    pmt::util::smrt::GenericAst const& _ast;
    pmt::util::smrt::StateNrType _state_nr_from = pmt::util::smrt::StateNrStart;
  };

 // --$ Other $--
 static void check_conflicts(Args args_);
};

}