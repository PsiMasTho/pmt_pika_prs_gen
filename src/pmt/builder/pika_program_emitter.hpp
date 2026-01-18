#pragma once

#include "pmt/builder/pika_program.hpp"
#include "pmt/util/skeleton_replacer_base.hpp"

#include <iosfwd>
#include <string>

namespace pmt::builder {

class PikaProgramEmitter : public pmt::util::SkeletonReplacerBase {
public:
 // -$ Types / Constants $-
 class Args {
 public:
  PikaProgram const& _program;
  std::string _header_include_path;
  std::ofstream& _output_header;
  std::ofstream& _output_source;
  std::ifstream& _header_skel_file;
  std::ifstream& _source_skel_file;
  std::string _class_name;
  std::string _namespace_name;
 };

private:
 // -$ Data $-
 Args _args;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit PikaProgramEmitter(Args args_);

 // --$ Other $--
 void emit();
};

}  // namespace pmt::builder
