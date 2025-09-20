#pragma once
#include <bitset>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace simcore {

// Build enc maps for fast greedy matching.
// - Tokens are assigned in the *given order* of rs.ligs (1..M).
// - has_by_len[L] contains all ligatures of length L (2..max_k).
struct EncMaps {
 std::unordered_map<std::string, unsigned char> token;     // lig -> id (1..M)
 std::vector<std::unordered_set<std::string>> has_by_len;  // index by length
 int max_k = 2;
};
EncMaps make_encmaps(const RuleSet& rs, int capN);

// Greedy encode a *single word* (letters only).
// - Longest ligature first (cap by em.max_k), left->right.
// - If no ligature matches at position i, omit letter if allow_omit && omit[ch] else emit letter.
// - Returns a compact string: normal letters are 'a'..'z'; ligatures are single bytes 1..M.
std::string encode_greedy(const std::string& w, const EncMaps& em, const std::array<bool, 26>& omit, bool allow_omit);

// Apply the greedy encoder to free-form text (keeps punctuation/whitespace).
// - Lowercases letters for matching/output (your script is lowercase).
// - Ligatures are replaced by a visible *number* token, e.g. "<1>", "<2>", ... in output.
// - Non-letters pass through unchanged.
// - If `legend_out` provided, it's filled with the ligature strings by their assigned numbers (1-based).
std::string render_apply_rule_to_text(const std::string& text, const RuleSet& rs, int capN, const std::string& token_prefix = "<", const std::string& token_suffix = ">", std::vector<std::string>* legend_out = nullptr);

struct WeightedWord {
 std::string _word;  // lowercase a..z only
 double _weight = 0.0;
};

struct SimStats {
 double _base_mass = 0.0;    // Σ f*|w|
 double _saved_total = 0.0;  // Σ f*(|w| - |final|)
 double _amb_frac = 0.0;     // Σ f(words reverted due to collisions)
 double pct_saved() const {
  return _base_mass > 0.0 ? (_saved_total / _base_mass * 100.0) : 0.0;
 }
};

SimStats simulate_greedy_then_revert(const RuleSet& rs, const std::vector<WeightedWord>& words, int capN);

// Helpers
auto parse_omit_letters(const std::string& letters_) -> std::bitset<26>;          // e.g. "aeiou"
auto parse_ligs_csv(const std::string& csv_) -> std::unordered_set<std::string>;  // e.g. "tion,ing,th"
auto tolower_letters_only(const std::string& s_) -> std::string;                  // letters -> lower, others kept
auto tolow_ascii(char c_) -> char;

}  // namespace simcore
