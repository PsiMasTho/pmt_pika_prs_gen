#include "pmt/meta/grammar_loader.hpp"

#include "pmt/meta/extract_hidden_expressions.hpp"
#include "pmt/meta/extract_repetition_expressions.hpp"
#include "pmt/meta/grammar_from_ast.hpp"
#include "pmt/meta/inline_rules.hpp"
#include "pmt/meta/pika_tables.hpp"
#include "pmt/meta/prune_grammar.hpp"
#include "pmt/meta/shrink_grammar.hpp"
#include "pmt/rt/pika_parser.hpp"
#include "pmt/util/levenshtein.hpp"

#include <map>
#include <set>

namespace pmt::meta {
using namespace pmt::rt;
namespace {
enum : size_t {
 MaxLevDistanceToReport = 3,
 MaxNamesToReport = 3,
};

void add_and_check_start_rules(Grammar& dest_, std::unordered_set<std::string> const& start_rule_names_) {
 std::unordered_set<std::string> const rule_names = dest_.get_rule_names();

 for (std::string const& start_rule_name : start_rule_names_) {
  if (rule_names.contains(start_rule_name)) {
   dest_.add_start_rule_name(start_rule_name);
   continue;
  }
  std::map<size_t, std::set<std::string>> lev_distances;
  pmt::util::Levenshtein lev;
  for (std::string const& rule_name : rule_names) {
   lev_distances[lev.distance(start_rule_name, rule_name)].insert(rule_name);
  }
  std::string error_msg = "Start rule not found: \"$" + start_rule_name + "\"";
  if (!lev_distances.empty() && lev_distances.begin()->first <= MaxLevDistanceToReport) {
   std::string delim;
   error_msg += ", did you mean: ";
   for (size_t i = 0; std::string const& rule_name : lev_distances.begin()->second) {
    if (i >= MaxNamesToReport) {
     error_msg += ", ...";
     break;
    } else {
     error_msg += std::exchange(delim, " OR ") + "\"$" + rule_name + "\"";
    }
    ++i;
   }
  }

  throw std::runtime_error(error_msg);
 }
}

}  // namespace

void GrammarLoader::push_start_rule(std::string start_rule_name_) {
 _start_rule_names.emplace(std::move(start_rule_name_));
}

void GrammarLoader::push_input(std::string_view input_) {
 _inputs.push_back(input_);
}

auto GrammarLoader::load_grammar() -> Grammar {
 assert(!_inputs.empty());

 pmt::meta::PikaTables const pika_tables;

 Ast::UniqueHandle ast_root = Ast::construct(Ast::Tag::Parent, ReservedIds::IdRoot);

 for (std::string_view const& input_cur : _inputs) {
  Ast::UniqueHandle ast_cur = PikaParser::memo_table_to_ast(PikaParser::populate_memo_table(input_cur, pika_tables), input_cur, pika_tables);
  if (!ast_cur) {
   throw std::runtime_error("Failed to parse grammar input.");
  }
  ast_root->give_child_at_back(std::move(ast_cur));
  ast_root->unpack(ast_root->get_children_size() - 1);
 }

 extract_hidden_expressions(*ast_root);
 extract_repetition_expressions(*ast_root);
 Grammar grammar = grammar_from_ast(ast_root);
 add_and_check_start_rules(grammar, _start_rule_names);
 prune_grammar(grammar);
 inline_rules(grammar);
 shrink_grammar(grammar);

 return grammar;
}

}  // namespace pmt::meta
