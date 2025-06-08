#include "pmt/parser/token.hpp"

namespace pmt::parser {

Token::Token(std::string_view token_, SourcePosition source_position_)
 : _token(token_)
 , _source_position(source_position_) {
}

auto Token::to_ast() const -> GenericAst::UniqueHandle {
  GenericAst::UniqueHandle ret = GenericAst::construct(GenericAst::Tag::String, _id);
  ret->set_string(std::string(_token));
  return ret;
}

}  // namespace pmt::util::smrt