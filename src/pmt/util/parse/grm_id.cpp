#include "pmt/util/parse/grm_id.hpp"

namespace pmt::util::parse {
auto GrmId::to_string(GenericAst::IdType id_) -> std::string {
  switch (id_) {
    case TkStringLiteral:
      return "String literal";
    case TkIdentifier:
      return "Identifier";
    case TkPipe:
      return "Pipe";
    case TkSemiColon:
      return "SemiColon";
    case TkEquals:
      return "Equals";
    case NtGrammar:
      return "Grammar";
    case NtDefinition:
      return "Definition";
    case NtChoices:
      return "Choices";
    case NtSequence:
      return "Sequence";
    case GenericAst::DefaultId:
      return "Default";
    case GenericAst::UninitializedId:
      return "Uninitialized";
    default:
      return "Unknown token: " + std::to_string(id_);
  }
}
}  // namespace pmt::util::parse