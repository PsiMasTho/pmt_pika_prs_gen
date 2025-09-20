// program short name: applyrule_tui
// Build: g++ -std=c++17 -O2 -o applyrule_tui applyrule_tui.cpp simcore.cpp
// Usage:
//   ./applyrule_tui --file=sample.txt --dict=words.txt \
//                   --lig="tion,ing,th,he" --omit="aeiou" --n=4 \
//                   [--prefix="<" --suffix=">"]
//
// What this tester does
//   • Loads a dictionary (one word per line, case-insensitive).
//   • With your ligatures/omits (and max lig length --n), it encodes every dict word greedily.
//   • If a dict word’s greedy encoding is UNIQUE among all dict words, we map that raw word
//     to its transformed text (ligatures as numbered tokens, omitted letters removed).
//   • Then we read the input text and replace only those words present in the unique map.
//     Unknown words or colliding dict words are left as-is.
//   • Legend "number -> ligature" is printed to stderr; transformed text to stdout.

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "argh/argh.h"
#include "simcore.hpp"

using namespace std;
using namespace simcore;

static string slurp(const string& path) {
 ifstream in(path, ios::binary);
 if (!in)
  throw runtime_error("cannot open input file: " + path);
 ostringstream ss;
 ss << in.rdbuf();
 return ss.str();
}

static vector<string> load_dictionary(const string& path) {
 ifstream in(path);
 if (!in)
  throw runtime_error("cannot open dictionary: " + path);
 vector<string> words;
 words.reserve(50000);
 string line;
 unordered_set<string> seen;
 while (getline(in, line)) {
  // lower and keep letters only
  string w;
  w.reserve(line.size());
  for (char c : line) {
   char cl = tolow_ascii(c);
   if (cl >= 'a' && cl <= 'z')
    w.push_back(cl);
  }
  if (!w.empty() && !seen.count(w)) {
   seen.insert(w);
   words.push_back(std::move(w));
  }
 }
 return words;
}

// Turn a greedy-encoded *word* (letters-only input) into visible tokens: letters stay as letters,
// ligatures become "<id>" (or custom prefix/suffix). We infer id range from EncMaps tokens.
static string render_word_compact_to_tokens(const string& compact, const EncMaps& em, const string& prefix, const string& suffix) {
 // Build reverse legend: token id -> present (we don't need names here, only range)
 unsigned char max_id = 0;
 for (const auto& kv : em.token) {
  if (kv.second > max_id)
   max_id = kv.second;
 }

 string out;
 out.reserve(compact.size() * 3);
 for (unsigned char uc : string(compact.begin(), compact.end())) {
  if (uc >= 'a' && uc <= 'z') {
   out.push_back(char(uc));
  } else if (uc >= 1 && uc <= max_id) {
   out.append(prefix);
   out.append(to_string(int(uc)));
   out.append(suffix);
  } else {
   // Shouldn't happen in our pipeline; fall back to raw char
   out.push_back(char(uc));
  }
 }
 return out;
}

int main(int argc, char** argv) {
 argh::parser cmdl(argc, argv);

 string inpath, dictpath, lig_csv, omit_letters, pref = "<", suff = ">";
 int N = 4;

 cmdl({"-f", "--file"}) >> inpath;
 cmdl({"-d", "--dict"}) >> dictpath;
 cmdl({"--lig"}) >> lig_csv;
 cmdl({"--omit"}) >> omit_letters;
 cmdl({"-n", "--n"}) >> N;
 cmdl({"--prefix"}) >> pref;
 cmdl({"--suffix"}) >> suff;

 if (inpath.empty() || dictpath.empty()) {
  cerr << "Usage: " << argv[0] << " --file=sample.txt --dict=words.txt --lig=\"tion,ing,th,he\" --omit=\"aeiou\" --n=4"
       << " [--prefix=\"<\" --suffix=\">\"]\n";
  return 1;
 }

 // Build rules (order of --lig defines numbering 1..M)
 RuleSet rs;
 rs.ligs = parse_ligs_csv(lig_csv);
 rs.omit = parse_omit_letters(omit_letters);

 // Enc maps once (shared for dict + rendering)
 EncMaps em = make_encmaps(rs, N);

 // Legend to stderr (number -> ligature)
 if (!rs.ligs.empty()) {
  // Build deterministic legend by given order (only those within cap N are assigned)
  vector<pair<int, string>> legend_pairs;
  legend_pairs.reserve(rs.ligs.size());
  for (const auto& lig : rs.ligs) {
   auto it = em.token.find(lig);
   if (it != em.token.end()) {
    legend_pairs.push_back({int(it->second), lig});
   }
  }
  sort(legend_pairs.begin(), legend_pairs.end());
  cerr << "# Legend (number -> ligature)\n";
  for (auto& p : legend_pairs) {
   cerr << p.first << " -> " << p.second << "\n";
  }
 }

 // Load dictionary and compute greedy encodings
 vector<string> dict_words;
 try {
  dict_words = load_dictionary(dictpath);
 } catch (const exception& e) {
  cerr << "Error: " << e.what() << "\n";
  return 1;
 }

 // 1) Compact encodings & buckets for collision detection
 unordered_map<string, int> enc_count;
 enc_count.reserve(dict_words.size() * 2);
 vector<string> compacts;
 compacts.resize(dict_words.size());

 for (size_t i = 0; i < dict_words.size(); ++i) {
  // encode_greedy expects letters only and lowercased
  compacts[i] = encode_greedy(dict_words[i], em, rs.omit, /*allow_omit=*/true);
  enc_count[compacts[i]]++;
 }

 // 2) Build mapping from raw dict word -> transformed tokens (only for UNIQUE encodings)
 unordered_map<string, string> dict_map;
 dict_map.reserve(dict_words.size());
 for (size_t i = 0; i < dict_words.size(); ++i) {
  const string& raw = dict_words[i];
  const string& cmp = compacts[i];
  if (enc_count[cmp] == 1) {
   // Render compact to tokenized string
   string rendered = render_word_compact_to_tokens(cmp, em, pref, suff);
   dict_map.emplace(raw, std::move(rendered));
  }
 }

 // 3) Read input text and rewrite words if present in dict_map; else leave as-is
 string text;
 try {
  text = slurp(inpath);
 } catch (const exception& e) {
  cerr << "Error: " << e.what() << "\n";
  return 1;
 }

 string out;
 out.reserve(text.size() * 2);
 const int n = (int)text.size();
 for (int i = 0; i < n;) {
  char c = text[i];
  char cl = tolow_ascii(c);
  if (cl >= 'a' && cl <= 'z') {
   int j = i;
   string raw;
   raw.reserve(32);
   while (j < n) {
    char cj = text[j];
    char cjl = tolow_ascii(cj);
    if (cjl >= 'a' && cjl <= 'z') {
     raw.push_back(cjl);
     ++j;
    } else
     break;
   }
   auto it = dict_map.find(raw);
   if (it != dict_map.end()) {
    out.append(it->second);
   } else {
    // unknown or collided -> write original as-is
    out.append(text.substr(i, j - i));
   }
   i = j;
  } else {
   out.push_back(c);
   ++i;
  }
 }

 // Optionally print some stats to stderr
 cerr << "# dict words: " << dict_words.size() << " | unique-encodable: " << dict_map.size() << " | collisions: " << (dict_words.size() - dict_map.size()) << "\n";

 // Final transformed text
 cout << out;
 return 0;
}
