#include "pmt/base/opaque_vector.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static char const* const WORDLIST[] = {
#include "wordlist-inl.h"
};

static size_t const WORDLIST_SIZE = sizeof(WORDLIST) / sizeof(WORDLIST[0]);

static char const* const COLORS[] = {
 "\033[31m", /* dark red */
 "\033[32m", /* dark green */
 "\033[33m", /* dark yellow */
 "\033[34m", /* dark blue */
 "\033[35m", /* dark magenta */
 "\033[36m", /* dark cyan */
 "\033[37m", /* light gray */
 "\033[91m", /* light red */
 "\033[92m", /* light green */
 "\033[93m", /* light yellow */
 "\033[94m", /* light blue */
 "\033[95m", /* light magenta */
 "\033[96m", /* light cyan */
 "\033[97m", /* white */
};

static size_t const COLORS_SIZE = sizeof(COLORS) / sizeof(COLORS[0]);

static char const* const RESET_COLOR = "\033[0m";

typedef struct options {
 size_t length;
 size_t number;
 bool   color;
} options;

static options
parse_args(int argc, char const* const* argv) {
 options opts = {7, 1, false};

 for (int i = 1; i < argc; ++i) {
  if (argv[i][0] == '-') {
   switch (argv[i][1]) {
   case 'l':
    opts.length = atoi(argv[++i]);
    break;
   case 'n':
    opts.number = atoi(argv[++i]);
    break;
   case 'c':
    opts.color = true;
    break;
   default:
    fprintf(stderr, "Unknown option: %s\n", argv[i]);
    exit(1);
   }
  }
 }

 return opts;
}

static size_t
gen_rnd_wordlist_index() {
 return rand() % WORDLIST_SIZE;
}

static size_t
gen_rnd_color_index() {
 return rand() % COLORS_SIZE;
}

// password format:
// <number><Word>_<word>_..._<word><exclamation mark>
// e.g.  l = 3:
// 0Foo_bar_baz!
auto
gen_password(int l) -> string {
 // initial number and exclamation mark
 vector<string> words;
 words.reserve(l);

 // fill in the words
 for (auto i = 0; i < l; ++i) {
  auto        idx_rng = get_rnd_wordlist_index_gen();
  std::string word    = g_wordlist[idx_rng()];
  words.push_back(word);
 }

 // capitalize the first letter of the first word
 words[0][0] = std::toupper(words[0][0]);

 // prepend the number
 auto num_rng = get_rnd_num_gen(0, 9);
 words.begin()->insert(0, to_string(num_rng()));

 // append the exclamation mark
 words.back().push_back('!');

 // join the words with underscores
 return fmt::format("{}", fmt::join(words, "_"));
}

int
main(int argc, char** argv) {
 srand(time(NULL));

 auto const cfg       = parse_args(argc, argv);
 auto       color_rng = get_rnd_color_gen();

 for (auto i = 0; i < cfg.number; ++i) {
  auto const pw = gen_password(cfg.length);
  if (cfg.color)
   fmt::print(fg(g_colors[color_rng()]), "{}\n", pw);
  else
   fmt::print("{}\n", pw);
 }
}
