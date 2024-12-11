#include "pmt/parserbuilder/grm_parser.hpp"

#include "pmt/parserbuilder/grm_parser-inl.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::parsert;

auto GrmParser::parse(GrmLexer& lexer_) -> GenericAst::UniqueHandle {
  GenericAst::UniqueHandle ret = GenericAst::construct(GenericAst::Tag::Children);

  void* parser = ParseAlloc(malloc);
  ParseInit(parser);
  while (!lexer_.is_eof()) {
    GenericAst::UniqueHandle token = lexer_.next_token();

    if (!token) {
      break;
    }

    Parse(parser, token->get_id(), std::move(token), &ret);
  }
  Parse(parser, 0, GenericAst::UniqueHandle(), &ret);

  ParseFree(parser, free);

  return ret;
}

}  // namespace pmt::parserbuilder