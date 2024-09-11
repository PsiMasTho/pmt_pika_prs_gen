#include "grammar.h"

grammar_info const GRAMMAR_INFO = {{
 {"file", AST_NODE_CHILDREN},
 {"section_lib", AST_NODE_CHILDREN},
 {"section_exe", AST_NODE_CHILDREN},
 {"section_custom", AST_NODE_CHILDREN},
 {"section_deps", AST_NODE_CHILDREN},
 {"identifier_stmnt", AST_NODE_CHILDREN},
 {"dependency_stmnt", AST_NODE_CHILDREN},
 {"identifier_body", AST_NODE_CHILDREN},
 {"dependency_body", AST_NODE_CHILDREN},
 {"dependency_list", AST_NODE_CHILDREN},
 {"identifier", AST_NODE_TOKEN},
}};
