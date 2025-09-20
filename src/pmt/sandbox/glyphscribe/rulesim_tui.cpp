// program short name: rulesim_tui
// Build: g++ -std=c++17 -O2 -o rulesim_tui rulesim_tui.cpp simcore.cpp
// Usage:
//   ./rulesim_tui --file=wordlist.txt \
//                 --n=4 --top=10 --beam=20 --kcap=400 --max-lig=5 --max-omit=5 \
//                 [--draw] [--min-edge=0.0] [--precision=6]
//
// Reads "word fraction", mines k-gram candidates (k=2..N), ranks letters by SLS,
// then beam-searches rulesets using the greedy-then-revert simulator from simcore.
// Optionally writes FSM DOTs (flow-normalized) for k=1..N.

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "argh/argh.h"
#include "simcore.hpp"

using namespace std;
using namespace simcore;

// ------- small utils -------
static inline string filter_az(const string& s) {
 string o;
 o.reserve(s.size());
 for (unsigned char c : s)
  if (c >= 'a' && c <= 'z')
   o.push_back(char(c));
 return o;
}
static inline vector<string> split_ws(const string& line) {
 vector<string> p;
 string t;
 istringstream iss(line);
 while (iss >> t)
  p.push_back(t);
 return p;
}

// Load & merge; also return baseline letters mass
static vector<WeightedWord> load_words_merge(const string& path, size_t& lines_read, size_t& skipped, double& baseline_letters_mass) {
 ifstream fin(path);
 if (!fin)
  throw runtime_error("cannot open input file");
 unordered_map<string, double> merged;
 merged.reserve(8192);
 string line;
 lines_read = skipped = 0;
 while (getline(fin, line)) {
  if (line.empty())
   continue;
  ++lines_read;
  auto parts = split_ws(line);
  if (parts.size() < 2) {
   ++skipped;
   continue;
  }
  string w = filter_az(tolower_letters_only(parts.front()));
  if (w.empty()) {
   ++skipped;
   continue;
  }
  double f = 0.0;
  try {
   f = stod(parts.back());
   if (!isfinite(f) || f < 0.0) {
    ++skipped;
    continue;
   }
  } catch (...) {
   ++skipped;
   continue;
  }
  merged[w] += f;
 }
 fin.close();
 vector<WeightedWord> out;
 out.reserve(merged.size());
 baseline_letters_mass = 0.0;
 for (auto& kv : merged) {
  if (kv.second > 0.0) {
   out.push_back({kv.first, kv.second});
   baseline_letters_mass += kv.second * (double)kv.first.size();
  }
 }
 return out;
}

// ---------- FSM mining (for DOT + occ_mass) ----------
struct Node {
 array<int, 26> next{};
 array<double, 26> w{};
 Node() {
  next.fill(-1);
  w.fill(0.0);
 }
};
struct FSM {
 vector<Node> nodes;
 vector<double> in_mass;
 FSM() {
  nodes.emplace_back();
 }
};
static inline int fsm_step(FSM& fsm, int s, char ch, double freq) {
 int idx = ch - 'a';
 if (idx < 0 || idx >= 26)
  return s;
 int t = fsm.nodes[s].next[idx];
 if (t == -1) {
  t = (int)fsm.nodes.size();
  fsm.nodes[s].next[idx] = t;
  fsm.nodes.emplace_back();
 }
 fsm.nodes[s].w[idx] += freq;
 return t;
}
static void fsm_normalize(FSM& fsm) {
 const size_t S = fsm.nodes.size();
 vector<array<double, 26>> raw(S), norm(S);
 for (size_t s = 0; s < S; ++s) {
  raw[s] = fsm.nodes[s].w;
  norm[s].fill(0.0);
 }
 fsm.in_mass.assign(S, 0.0);
 fsm.in_mass[0] = 1.0;
 for (size_t s = 0; s < S; ++s) {
  double R = 0.0;
  for (int j = 0; j < 26; ++j)
   R += raw[s][j];
  if (R <= 0.0)
   continue;
  for (int j = 0; j < 26; ++j) {
   double r = raw[s][j];
   if (r <= 0.0)
    continue;
   double wn = fsm.in_mass[s] * (r / R);
   norm[s][j] = wn;
   int t = fsm.nodes[s].next[j];
   if (t != -1)
    fsm.in_mass[t] += wn;
  }
 }
 for (size_t s = 0; s < S; ++s)
  for (int j = 0; j < 26; ++j) {
   double v = norm[s][j];
   fsm.nodes[s].w[j] = (fabs(v) < 1e-15 ? 0.0 : v);
  }
}
static void fsm_write_dot(const FSM& fsm, int k, double min_edge, int precision) {
 double maxw = 0.0;
 size_t ec = 0;
 for (size_t s = 0; s < fsm.nodes.size(); ++s)
  for (int j = 0; j < 26; ++j) {
   double w = fsm.nodes[s].w[j];
   if (w > 0.0) {
    maxw = max(maxw, w);
    ++ec;
   }
  }
 auto pen = [&](double w) {
  if (maxw <= 0.0)
   return 1.0;
  double x = w / maxw;
  return 0.6 + 5.4 * x;
 };
 ostringstream oss;
 oss << "fsm_k" << k << ".dot";
 ofstream out(oss.str());
 if (!out) {
  cerr << "Error: cannot write " << oss.str() << "\n";
  return;
 }
 out << "digraph FSM_k" << k << " {\n  rankdir=LR;\n  graph [splines=true, overlap=false];\n  node [shape=circle, fontsize=10];\n  edge [fontsize=9];\n\n";
 for (size_t s = 0; s < fsm.nodes.size(); ++s) {
  if (s == 0)
   out << "  " << s << " [shape=doublecircle, style=filled, fillcolor=\"#eeeeff\"];\n";
  else
   out << "  " << s << ";\n";
 }
 out << "\n" << fixed << setprecision(precision);
 for (size_t s = 0; s < fsm.nodes.size(); ++s) {
  const auto& nd = fsm.nodes[s];
  for (int j = 0; j < 26; ++j) {
   double w = nd.w[j];
   if (w <= min_edge)
    continue;
   int t = nd.next[j];
   if (t == -1)
    continue;
   out << "  " << s << " -> " << t << " [label=\"" << char('a' + j) << " / " << w << "\", penwidth=" << pen(w) << "];\n";
  }
 }
 out << "}\n";
 out.close();
 cerr << "Wrote " << oss.str() << " (states=" << fsm.nodes.size() << ", edges=" << ec << ")\n";
}

struct FSMMineResult {
 vector<FSM> fsms;                           // 1..N
 vector<unordered_map<string, double>> occ;  // per-k occ_mass (k>=2)
};
static FSMMineResult build_fsms_and_occ(const vector<WeightedWord>& words, int N) {
 FSMMineResult r;
 r.fsms.resize(N + 1);
 r.occ.resize(N + 1);
 for (int k = 1; k <= N; ++k) {
  FSM fsm;
  auto& occ = r.occ[k];
  for (const auto& wi : words) {
   const string& w = wi._word;
   int n = (int)w.size();
   if (n < k)
    continue;
   for (int i = 0; i <= n - k; ++i) {
    int s = 0;
    for (int j = 0; j < k; ++j)
     s = fsm_step(fsm, s, w[i + j], wi._weight);
    if (k >= 2)
     occ[w.substr(i, k)] += wi._weight;
   }
  }
  fsm_normalize(fsm);
  r.fsms[k] = std::move(fsm);
 }
 return r;
}

// Letter SLS
struct LetterScore {
 char c;
 double safe_mass = 0.0;
};
static vector<LetterScore> rank_letters_SLS(const vector<WeightedWord>& words) {
 struct PW {
  string w;
  double f;
  array<int, 26> cnt{};
 };
 vector<PW> W;
 W.reserve(words.size());
 for (auto& wi : words) {
  PW x{wi._word, wi._weight, {}};
  for (char ch : wi._word)
   x.cnt[ch - 'a']++;
  W.push_back(std::move(x));
 }
 vector<LetterScore> scores;
 scores.reserve(26);
 for (int li = 0; li < 26; ++li) {
  char L = char('a' + li);
  unordered_map<string, int> buckets;
  buckets.reserve(W.size() * 2);
  vector<string> reduced(W.size());
  for (size_t i = 0; i < W.size(); ++i) {
   const string& w = W[i].w;
   string r;
   r.reserve(w.size());
   for (char ch : w)
    if (ch != L)
     r.push_back(ch);
   reduced[i] = std::move(r);
   buckets[reduced[i]]++;
  }
  double safe = 0.0;
  for (size_t i = 0; i < W.size(); ++i) {
   if (buckets[reduced[i]] == 1) {
    int c = W[i].cnt[li];
    if (c > 0)
     safe += W[i].f * c;
   }
  }
  scores.push_back({L, safe});
 }
 sort(scores.begin(), scores.end(), [](const LetterScore& a, const LetterScore& b) {
  if (a.safe_mass != b.safe_mass)
   return a.safe_mass > b.safe_mass;
  return a.c < b.c;
 });
 return scores;
}

// Candidate ligatures by potential savings
struct KGramRow {
 int k;
 string gram;
 double occ = 0.0;
 double pot = 0.0;
};
static vector<KGramRow> select_kgram_candidates(const vector<unordered_map<string, double>>& occ, int N, int kcap) {
 vector<KGramRow> cand;
 for (int k = 2; k <= N; ++k) {
  vector<KGramRow> rows;
  rows.reserve(occ[k].size());
  for (const auto& kv : occ[k])
   rows.push_back({k, kv.first, kv.second, (double)(k - 1) * kv.second});
  sort(rows.begin(), rows.end(), [](const KGramRow& a, const KGramRow& b) {
   if (a.pot != b.pot)
    return a.pot > b.pot;
   if (a.k != b.k)
    return a.k > b.k;
   return a.gram < b.gram;
  });
  if ((int)rows.size() > kcap)
   rows.resize(kcap);
  cand.insert(cand.end(), rows.begin(), rows.end());
 }
 return cand;
}

// Beam-search with simcore::simulate_greedy_then_revert
struct NodeRS {
 RuleSet rs;
 SimStats st;
};
static string rs_key(const RuleSet& rs) {
 // canonical key for dedup (sort ligs so order doesn't explode the state space)
 vector<string> L = rs._ligs;
 sort(L.begin(), L.end());
 string k = "L=";
 for (size_t i = 0; i < L.size(); ++i) {
  if (i)
   k.push_back('|');
  k += L[i];
 }
 k += ";O=";
 for (int i = 0; i < 26; ++i)
  if (rs.omit[i])
   k.push_back(char('a' + i));
 return k;
}

static vector<NodeRS> search_rules(const vector<WeightedWord>& words, const vector<KGramRow>& cand_ligs, const vector<LetterScore>& cand_letters, int N, int max_lig, int max_omit, int beam, int top_report) {
 auto better = [](const NodeRS& a, const NodeRS& b) {
  if (a.st.pct_saved() != b.st.pct_saved())
   return a.st.pct_saved() > b.st.pct_saved();
  int ca = (int)a.rs._ligs.size(), cb = (int)b.rs._ligs.size();
  if (ca != cb)
   return ca < cb;
  return a.rs._ligs < b.rs._ligs;
 };

 // Convert to simcore::WeightedWord
 vector<simcore::WeightedWord> W;
 W.reserve(words.size());
 for (auto& x : words)
  W.push_back({x.w, x.f});

 NodeRS root;
 root.rs.omit.fill(false);
 root.rs._ligs.clear();
 root.st = simulate_greedy_then_revert(root.rs, W, N);

 vector<NodeRS> beamset{root};
 unordered_set<string> seen;
 seen.reserve(200000);
 seen.insert(rs_key(root.rs));

 const int DEPTH = max_lig + max_omit;
 for (int depth = 0; depth < DEPTH; ++depth) {
  vector<NodeRS> next;
  for (const auto& parent : beamset) {
   // add one ligature
   if ((int)parent.rs._ligs.size() < max_lig) {
    int proposals = 0;
    for (const auto& kg : cand_ligs) {
     if (find(parent.rs._ligs.begin(), parent.rs._ligs.end(), kg.gram) != parent.rs._ligs.end())
      continue;
     RuleSet child = parent.rs;
     child._ligs.push_back(kg.gram);
     string key = rs_key(child);
     if (seen.insert(key).second) {
      NodeRS n;
      n.rs = std::move(child);
      n.st = simulate_greedy_then_revert(n.rs, W, N);
      next.push_back(std::move(n));
      if (++proposals >= beam * 2)
       break;
     }
    }
   }
   // add one omitted letter
   if ((int)count(parent.rs.omit.begin(), parent.rs.omit.end(), true) < max_omit) {
    int proposals = 0;
    for (const auto& ls : cand_letters) {
     int idx = ls.c - 'a';
     if (parent.rs.omit[idx])
      continue;
     RuleSet child = parent.rs;
     child.omit[idx] = true;
     string key = rs_key(child);
     if (seen.insert(key).second) {
      NodeRS n;
      n.rs = std::move(child);
      n.st = simulate_greedy_then_revert(n.rs, W, N);
      next.push_back(std::move(n));
      if (++proposals >= beam * 2)
       break;
     }
    }
   }
  }
  if (next.empty())
   break;
  sort(next.begin(), next.end(), better);
  if ((int)next.size() > beam)
   next.resize(beam);
  beamset.swap(next);
 }

 sort(beamset.begin(), beamset.end(), better);
 if ((int)beamset.size() > top_report)
  beamset.resize(top_report);
 return beamset;
}

// ---------- main ----------
int main(int argc, char** argv) {
 argh::parser cmdl(argc, argv);
 string inpath;
 int N = 4;
 int TOP = 10;
 int BEAM = 20;
 int KCAP = 400;
 int MAX_LIG = 5;
 int MAX_OMIT = 5;
 bool DRAW = false;
 double MIN_EDGE = 0.0;
 int PREC = 6;

 cmdl({"-f", "--file"}) >> inpath;
 cmdl({"-n", "--n"}) >> N;
 cmdl({"--top"}) >> TOP;
 cmdl({"--beam"}) >> BEAM;
 cmdl({"--kcap"}) >> KCAP;
 cmdl({"--max-lig"}) >> MAX_LIG;
 cmdl({"--max-omit"}) >> MAX_OMIT;
 if (cmdl[{"--draw"}])
  DRAW = true;
 cmdl({"--min-edge"}) >> MIN_EDGE;
 cmdl({"--precision"}) >> PREC;

 if (inpath.empty() || N < 2) {
  cerr << "Usage: " << argv[0]
       << " --file=wordlist.txt --n=4 [--top=10] [--beam=20] [--kcap=400] "
          "[--max-lig=5 --max-omit=5] [--draw] [--min-edge=0.0] [--precision=6]\n";
  cerr << "Note: --n must be >= 2.\n";
  return 1;
 }

 size_t lines = 0, skipped = 0;
 double base_letters = 0.0;
 vector<WeightedWord> words;
 try {
  words = load_words_merge(inpath, lines, skipped, base_letters);
 } catch (const exception& e) {
  cerr << "Error: " << e.what() << "\n";
  return 1;
 }
 cerr << "# Lines read: " << lines << " | unique merged: " << words.size() << " | skipped: " << skipped << "\n";
 cerr << "# Baseline letters mass (Σ f·|w|): " << fixed << setprecision(6) << base_letters << "\n";

 // Mine FSMs + occ_mass (and optionally DOTs)
 auto mined = build_fsms_and_occ(words, N);
 if (DRAW) {
  for (int k = 1; k <= N; ++k)
   fsm_write_dot(mined.fsms[k], k, MIN_EDGE, PREC);
 }

 // Build candidate pools
 vector<KGramRow> cand_ligs = select_kgram_candidates(mined.occ, N, KCAP);
 auto cand_letters = rank_letters_SLS(words);

 // Search best rules
 auto best = search_rules(words, cand_ligs, cand_letters, N, MAX_LIG, MAX_OMIT, BEAM, TOP);

 // Report
 cout.setf(std::ios::fixed);
 cout << "## Top " << best.size() << " rulesets (greedy encode, then revert collided words to plain)\n";
 cout << "rank,pct_saved,saved_total,baseline,ambiguous_fraction,ligatures,omit\n";
 for (size_t i = 0; i < best.size(); ++i) {
  const auto& n = best[i];
  string ligs;
  for (size_t j = 0; j < n.rs._ligs.size(); ++j) {
   if (j)
    ligs.push_back('|');
   ligs += n.rs._ligs[j];
  }
  string omit;
  for (int j = 0; j < 26; ++j)
   if (n.rs.omit[j])
    omit.push_back(char('a' + j));
  cout << (i + 1) << "," << setprecision(4) << n.st.pct_saved() << "," << setprecision(8) << n.st.saved_total << "," << setprecision(8) << n.st.base_mass << "," << setprecision(8) << n.st.amb_frac << "," << ligs << "," << omit << "\n";
 }

 return 0;
}
