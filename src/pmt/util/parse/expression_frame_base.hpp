#pragma once

#include "pmt/util/parse/fa_part.hpp"
#include "pmt/util/parse/generic_ast.hpp"

#include <memory>
#include <stack>

namespace pmt::util::parse {

class ExpressionFrameBase : public std::enable_shared_from_this<ExpressionFrameBase> {
 public:
  // - Public Types -
  using FrameHandle = std::shared_ptr<ExpressionFrameBase>;
  using CallstackType = std::stack<FrameHandle>;

  class Captures {
   public:
    FaPart& _ret_part;
    Fa& _result;
  };

  // - Lifetime -
  virtual ~ExpressionFrameBase() = default;

  // - Public member functions -
  virtual auto get_id() const -> GenericAst::IdType = 0;
  virtual void process(CallstackType& callstack_, Captures& captures_) = 0;
};

}  // namespace pmt::util::parse