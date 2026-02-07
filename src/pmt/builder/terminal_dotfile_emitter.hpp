#pragma once

#include "pmt/builder/state_machine.hpp"
#include "pmt/rt/primitives.hpp"
#include "pmt/util/skeleton_replacer_base.hpp"

#include <functional>
#include <iosfwd>
#include <string>

namespace pmt::builder {

class TerminalDotfileEmitter : public pmt::util::SkeletonReplacerBase {
 // -$ Types / Constants $-
public:
 using FinalIdToStringFn = std::function<std::string(pmt::rt::IdType)>;
 class Args {
 public:
  FinalIdToStringFn _final_id_to_string_fn;
  std::string _terminal_dotfile_skel;
  StateMachine const& _state_machine;
  std::ostream* _output_terminal_dotfile;
 };

private:
 // -$ Data $-
 Args _args;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 TerminalDotfileEmitter(Args args_);

 // -$ Other $-
 void write_dot();
};

}  // namespace pmt::builder
