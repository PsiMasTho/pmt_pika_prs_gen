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

  std::ostream* _output_id_strings = nullptr;
  std::string _id_strings_skel;

  std::ostream* _output_id_constants = nullptr;
  std::string _id_constants_skel;
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
