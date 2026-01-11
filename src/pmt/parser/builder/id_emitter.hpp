#pragma once

#include "pmt/parser/grammar/id_table.hpp"
#include "pmt/util/skeleton_replacer_base.hpp"

#include <iosfwd>

namespace pmt::parser::builder {

class IdEmitter : public pmt::util::SkeletonReplacerBase {
public:
 // -$ Types / Constants $-
 class Args {
 public:
  pmt::parser::grammar::IdTable const& _id_table;

  std::ofstream& _strings_output_file;
  std::ifstream& _strings_skel_file;

  std::ofstream& _constants_output_file;
  std::ifstream& _constants_skel_file;
 };

private:
 // -$ Data $-
 Args _args;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit IdEmitter(Args args_);

 // --$ Other $--
 void emit();
};

}  // namespace pmt::parser::builder
