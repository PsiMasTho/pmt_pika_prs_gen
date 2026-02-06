#pragma once

#include "pmt/meta/id_table.hpp"
#include "pmt/util/skeleton_replacer_base.hpp"

#include <iosfwd>

namespace pmt::builder {

class IdEmitter : public pmt::util::SkeletonReplacerBase {
public:
 // -$ Types / Constants $-
 class Args {
 public:
  pmt::meta::IdTable const& _id_table;

  std::ostream* _strings_output_file = nullptr;
  std::string _strings_skel;

  std::ostream* _constants_output_file = nullptr;
  std::string _constants_skel;
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

}  // namespace pmt::builder
