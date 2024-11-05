#include "pmt/util/parse/grm_parser.hpp"

#include "pmt/util/parse/grm_parser-inl.hpp"

namespace pmt::util::parse {

auto GrmParser::parse(GrmLexer& lexer_) -> GenericAst::UniqueHandle {
  GenericAst::UniqueHandle ret = GenericAst::construct(GenericAst::Tag::Children);

  void* parser = ParseAlloc(malloc);
  ParseInit(parser);
  while (!lexer_.is_eof()) {
    GenericAst::UniqueHandle token = lexer_.next_token();
    Parse(parser, token->get_id(), std::move(token), &ret);
  }
  Parse(parser, 0, GenericAst::UniqueHandle(), &ret);

  ParseFree(parser, free);

  return ret;
}

}  // namespace pmt::util::parse