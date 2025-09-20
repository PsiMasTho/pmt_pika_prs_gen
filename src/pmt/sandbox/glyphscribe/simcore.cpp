// simcore.cpp
#include "simcore.hpp"
#include <algorithm>
#include <cctype>
#include <unordered_set>

namespace simcore {

EncMaps make_encmaps(const RuleSet& rs, int capN) {
 EncMaps m;
 int maxk = 2;
 m.has_by_len.assign(std::max((int)rs._ligs.size(), capN) + 5, {});  // safe size; we only use 0..capN
 unsigned char t = 1;
 for (const auto& s : rs._ligs) {
  if (s.size() < 2 || (capN > 0 && (int)s.size() > capN))
   continue;
  m.token[s] = t++;
  if ((int)m.has_by_len.size() <= (int)s.size())
   m.has_by_len.resize((int)s.size() + 1);
  m.has_by_len[s.size()].insert(s);
  maxk = std::max(maxk, (int)s.size());
 }
 m.max_k = (capN > 0) ? std::min(maxk, capN) : maxk;
 return m;
}

std::string encode_greedy(const std::string& w, const EncMaps& em, const std::array<bool, 26>& omit, bool allow_omit) {
 std::string out;
 out.reserve(w.size());
 const int n = (int)w.size();
 for (int i = 0; i < n;) {
  bool matched = false;
  int rem = n - i;
  for (int L = std::min(em.max_k, rem); L >= 2; --L) {
   const auto& setL = em.has_by_len[L];
   if (setL.empty())
    continue;
   std::string s = w.substr(i, L);
   if (setL.find(s) != setL.end()) {
    // push token byte (1..M) as a char
    out.push_back(char(em.token.at(s)));
    i += L;
    matched = true;
    break;
   }
  }
  if (!matched) {
   char ch = w[i];
   if (allow_omit && ch >= 'a' && ch <= 'z' && omit[ch - 'a']) {
    // omit (emit nothing)
   } else {
    out.push_back(ch);
   }
   ++i;
  }
 }
 return out;
}

std::string render_apply_rule_to_text(const std::string& text, const RuleSet& rs, int capN, const std::string& token_prefix, const std::string& token_suffix, std::vector<std::string>* legend_out) {
 EncMaps em = make_encmaps(rs, capN);
 // Build legend: index 1..M
 std::vector<std::string> legend(1, "");  // index 0 unused
 legend.reserve(1 + rs._ligs.size());
 // Preserve numbering in the order ligs were provided
 for (const auto& lig : rs._ligs) {
  if (em.token.find(lig) != em.token.end()) {
   // Ensure legend is large enough
   unsigned char id = em.token[lig];
   if ((int)legend.size() <= (int)id)
    legend.resize(id + 1);
   legend[id] = lig;
  }
 }
 if (legend_out)
  *legend_out = legend;

 std::string out;
 out.reserve(text.size() * 2);

 const int n = (int)text.size();
 for (int i = 0; i < n;) {
  char c = text[i];
  char cl = tolow_ascii(c);

  if (cl >= 'a' && cl <= 'z') {
   // Peek ahead on lowercase buffer for ligs / omit
   // Build a temp lowercase letter-only view per step
   // For speed, match directly in the original string using tolower on the fly
   bool matched = false;
   int rem = n - i;
   // Try longest ligature
   for (int L = std::min(em.max_k, rem); L >= 2; --L) {
    // Assemble lower substring of length L, break if any non-letter interrupts
    std::string seg;
    seg.reserve(L);
    bool ok = true;
    for (int k = 0; k < L; ++k) {
     char ck = tolow_ascii(text[i + k]);
     if (ck < 'a' || ck > 'z') {
      ok = false;
      break;
     }
     seg.push_back(ck);
    }
    if (!ok)
     continue;
    const auto& setL = (L < (int)em.has_by_len.size()) ? em.has_by_len[L] : std::unordered_set<std::string>{};
    if (!setL.empty()) {
     auto it = setL.find(seg);
     if (it != setL.end()) {
      // Emit number token
      unsigned char id = em.token.at(seg);
      out.append(token_prefix);
      out.append(std::to_string((int)id));
      out.append(token_suffix);
      i += L;
      matched = true;
      break;
     }
    }
   }
   if (!matched) {
    // If not a ligature start, maybe omit this single letter
    if (rs.omit[cl - 'a']) {
     // omit
     ++i;
    } else {
     out.push_back(cl);
     ++i;
    }
   }
  } else {
   // Not a letter: pass through unchanged
   out.push_back(c);
   ++i;
  }
 }

 return out;
}

SimStats simulate_greedy_then_revert(const RuleSet& rs, const std::vector<WeightedWord>& words, int capN) {
 SimStats st{};
 for (const auto& w : words)
  st.base_mass += w.f * (double)w.w.size();

 EncMaps em = make_encmaps(rs, capN);

 struct Rec {
  std::string enc;
  int len_enc = 0;
  int len_plain = 0;
  double f = 0.0;
 };
 std::vector<Rec> rec;
 rec.reserve(words.size());

 // Greedy encode (no collision checks)
 for (const auto& wi : words) {
  Rec r;
  r.f = wi.f;
  std::string enc = encode_greedy(wi.w, em, rs.omit, /*allow_omit=*/true);
  r.len_enc = (int)enc.size();
  r.enc = std::move(enc);
  r.len_plain = (int)wi.w.size();
  rec.push_back(std::move(r));
 }

 // Find duplicates
 std::unordered_map<std::string, int> bucket;
 bucket.reserve(rec.size() * 2);
 for (const auto& r : rec)
  bucket[r.enc]++;

 // Compute savings; revert collided to plain
 for (const auto& r : rec) {
  bool collided = (bucket[r.enc] >= 2);
  if (collided) {
   st.amb_frac += r.f;
   // saved = 0
  } else {
   st.saved_total += r.f * (double)(r.len_plain - r.len_enc);
  }
 }

 return st;
}

auto parse_omit_letters(const std::string& letters) -> std::bitset<26> {
 std::bitset<26> omit{};
 for (char c : letters) {
  char cl = tolow_ascii(c);
  if (cl >= 'a' && cl <= 'z')
   omit[cl - 'a'] = true;
 }
 return omit;
}

std::vector<std::string> parse_ligs_csv(const std::string& csv) {
 std::vector<std::string> out;
 std::string cur;
 for (char c : csv) {
  if (c == ',' || c == ' ') {
   if (!cur.empty()) {
    out.push_back(cur);
    cur.clear();
   }
  } else {
   cur.push_back(tolow_ascii(c));
  }
 }
 if (!cur.empty())
  out.push_back(cur);
 // filter ligs of length >= 2 and a..z only
 auto is_ok = [](const std::string& s) {
  if (s.size() < 2)
   return false;
  for (char ch : s)
   if (!(ch >= 'a' && ch <= 'z'))
    return false;
  return true;
 };
 std::vector<std::string> ok;
 ok.reserve(out.size());
 for (auto& s : out)
  if (is_ok(s))
   ok.push_back(s);
 return ok;
}

std::string tolower_letters_only(const std::string& s) {
 std::string o;
 o.reserve(s.size());
 for (char c : s) {
  if (c >= 'A' && c <= 'Z')
   o.push_back(char(c - 'A' + 'a'));
  else
   o.push_back(c);
 }
 return o;
}

auto tolow_ascii(char c) -> char {
 return (c >= 'A' && c <= 'Z') ? char(c - 'A' + 'a') : c;
}

}  // namespace simcore
