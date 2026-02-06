# C++ Pika Parser Generator

A **C++20 parser generator** implementing the **Pika parsing algorithm** described in the paper:  
[Pika parsing: reformulating packrat parsing as a dynamic programming algorithm solves the left recursion and error recovery problems. Luke A. D. Hutchison, May 2020.](https://arxiv.org/abs/2005.06444)

---

## Dependencies
### This repository:
- C++20 compiler
- CMake ≥ 3.9
### The generated parsers
- C++20 compiler
- `pmt_pika_prs_gen_rt` (library + headers, includes the `pmt::rt` AST runtime)

## Building
This project is built with CMake.

To configure, build, and install (generator-agnostic):
```bash
# Configure (use the default install prefix)
cmake -S . -B build

# Or set the install prefix explicitly
# cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/your/prefix

cmake --build build

# Install CLI + runtime library/headers
cmake --build build --target install

# Other useful targets
cmake --build build --target make-example  # build the runnable example in ./example
cmake --build build --target install-skel  # install only the skeleton templates from ./skel
```

## CLI (pmt_pika_prs_gen_cli)
### Arguments
Required (normal mode):
* `--input-grammar-file`
* `--header-include-filename`
* `--output-header-file`
* `--output-source-file`
* `--id-strings-output-file`
* `--id-constants-output-file`

Optional:
* `--header-skel-file`
* `--source-skel-file`
* `--class-name`
* `--namespace-name`
* `--id-strings-skel-file`
* `--id-constants-skel-file`
* `--output-grammar`
* `--output-clauses`
* `--output-dotfile`
* `--terminal-graph-skel-file` (only used when `--output-dotfile` is set)
* `--skel-dir`

Test mode:
* Supplying `--input-test-file` runs the CLI in test mode (parse the test input and print the AST).
* Required in test mode: `--input-grammar-file`, `--input-test-file`.
* Optional in test mode: `--output-grammar`, `--output-clauses`, `--output-dotfile`, and any C++ generation args.
* C++ outputs are only emitted when their output args are supplied.
* Pika tables outputs require: `--header-include-filename`, `--output-header-file`, `--output-source-file`.
* ID outputs require: `--id-strings-output-file`, `--id-constants-output-file`.

### Generated files:
- **Pika tables header** (`--output-header-file`)
  - Declares a class derived from `pmt::rt::PikaTablesBase`.
  - The class name/namespace can be overridden via `--class-name` and `--namespace-name`.
- **Pika tables source** (`--output-source-file`)
  - Defines the table data and implements the `PikaTablesBase` interface.
  - Includes the generated header using `--header-include-filename`.
- **ID constants include** (`--id-constants-output-file`)
  - A C++ include fragment containing enum entries (not a standalone header).
  - Intended to be included inside an `enum` (see `src/pmt/meta/ids.hpp` for a working pattern).
- **ID strings include** (`--id-strings-output-file`)
  - A C++ include fragment containing the ID constants as string literals (not a standalone header).
  - Intended to be included inside a string table (see `src/pmt/meta/ids.cpp` for a working pattern).

Optional debug outputs:
- `--output-grammar`: write the normalized grammar after parsing/processing.
- `--output-clauses`: write a dump of the internal clause graph.
- `--output-dotfile`: write a Graphviz `.dot` file of the terminal state machine.

## Usage
To parse with your generated tables:
1. Compile the generated tables `.cpp` and include the generated headers where needed.
2. Link against `pmt_pika_prs_gen_rt`.
3. Use `pmt::rt::PikaParser` with an instance of your generated `PikaTablesBase` implementation.

See `example/` for a small self contained project that:
- shows how to use the generated files together (example_tables.{hpp,cpp}, id_constants-inl.hpp, id_strings-inl.hpp)
- parses a string via `pmt::rt::PikaParser`
- prints the AST via `pmt::rt::AstToString`
- shows how to manually traverse the AST

`example/grammar.pika` is the source grammar. The repo already includes all the generated files, but you can regenerate them with `scripts/gen_example.py`
This requires `pmt_pika_prs_gen_cli` to be available in your `PATH` (build/install the CLI first).

The runtime library also includes am Ast class that supports arbitrarily deep trees.

- `pmt::rt::Ast`: Core tree node type. Construct via `Ast::construct` and manage through `Ast::UniqueHandle` (stack allocation or manual `new`/`delete` is invalid). Use `give_child_at*` / `take_child_at*` to insert or remove nodes from a parent.
- `Ast::merge`: Replaces the current node with a `String` node whose contents are the depth-first concatenation of all string leaves in its subtree, preserving the original node id.
- `Ast::unpack`: If the child at a given index is a parent, removes that child and splices its children into the current node at the same position.
- `pmt::rt::Path`: Index path used with `resolve` to locate nodes in a tree, avoiding raw pointers that may be invalidated by tree edits.
- `pmt::rt::AstToString`: Pretty-prints a tree with indentation and a customizable id-to-string mapping.
- `pmt::rt::ReservedIds`: Helpers for sentinel ids such as `IdRoot`, `IdDefault`, and `IdUninitialized`.

## Grammar description
#### Statements
- Start rule (required): `@start = $S;`
- Productions:
  - `$S = e;`
  - `$S<params> = e;`

Identifiers are `$`-prefixed (e.g. `$S`, `$expr`, `$some_name`) and may contain letters, digits, and underscores.
Identifiers must start with a letter or `_` (after the `$`).

Whitespace, newlines, and comments may appear between tokens:
* `// ...` single-line comments
* `/* ... */` multi-line comments (not nested)

#### Parameters
`params` is a comma-separated list of zero or more assignments:
* `merge = true | false`
* `hide = true | false`
* `unpack = true | false`
* `id = "IdString"` (identifier-like string, will become a C++ enum constant in the generated code; e.g. `"MyNode_01"`)
* `display_name = "SomeName"`

#### Atoms (terminals and nonterminals)
* `$identifier` nonterminal reference
* `"text"` string literal
* `base#value` integer literal (base is decimal; value digits are `0-9`/`a-z` case-insensitive; base must be in `[2, 36]`)
* `[ "a".."z", "Q", 16#5f ]` charset (comma-separated items; items are character literals like `"Q"` or integer literals; ranges use `..`)
* `epsilon` succeeds without consuming input
* `eof` succeeds only at end-of-input, does not consume input. It is optional and not required for successful parsing.

Notes:
- There are no escape sequences; anything that cannot appear inside a string must be written as an integer literal. Check the meta-grammar to see what is allowed inside a string.
- Character values are bytes. Any integer literal used as a character value (e.g. in a charset item/range) must be in the `uint8_t` range `[0, 255]`.

#### Expressions
* `e1 e2 e3` sequence
* `e1 | e2 | e3` ordered choice
* `(e)` grouping

#### Postfix operators
These apply to a single atom or a parenthesized group:
* `e~` hide: suppress `e` in the AST (useful for delimiters/whitespace/comments)
* `e!` negative lookahead: succeeds if `e` fails; consumes no input
* `e&` positive lookahead: succeeds if `e` succeeds; consumes no input
* `e?` optional (same as `e | epsilon`)
* `e*` zero-or-more (greedy)
* `e+` one-or-more (greedy)
* `e{I}` exactly `I` repetitions
* `e{I,}` at least `I` repetitions (greedy)
* `e{,J}` at most `J` repetitions
* `e{I,J}` between `I` and `J` repetitions (`I <= J`)
* `e{,}` same as `e*`

`I`/`J` are integer literals (e.g. `10#2`).

The meta-grammar (the grammar that defines this syntax) is in `src/pmt/meta/grammar.pika`.

## Parse success
- Parsing is considered to have succeeded if starting rule matches from position 0 to the end of input.

## Implementation notes
- Parsing and AST construction are iterative (no recursion), so very deep ASTs/expressions are supported (subject to available memory).
- As an optimization, a DFA is constructed from all terminals and emitted as compact transition tables (see `--output-dotfile`).

## Skeleton files
- `pmt_pika_prs_gen_cli` uses skeleton templates to generate the emitted C++ (and optional debug outputs).
- The default skeletons live under `skel/` in this repo
- You can point the CLI at a different skeleton root with `--skel-dir /path/to/skel`.
- You can override individual templates with:
  - `--header-skel-file`
  - `--source-skel-file`
  - `--id-strings-skel-file`
  - `--id-constants-skel-file`
  - `--terminal-graph-skel-file` 
- To install only the skeleton templates, build the `install-skel` target (see the Build section above).

## Vscode Extension
- A minimal vscode extension that provides syntax hilighting for `.pika` files is in `pika-vscode/`
- Manual install:
  - `mkdir -p ~/.vscode/extensions/pika-lang-0.0.1`
  - `cp -R pika-vscode/* ~/.vscode/extensions/pika-lang-0.0.1/`
