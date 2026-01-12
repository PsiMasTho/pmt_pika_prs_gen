0 {
  kind: Identifier;
  children: [1];
  identifier: %grammar;
  seed_parents: [];
  canMatchZeroChars: false
}
1 {
  kind: Sequence;
  children: [2, 31, 298, 303];
  seed_parents: [0];
  canMatchZeroChars: false
}
2 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [1];
  canMatchZeroChars: true
}
3 {
  kind: Choice;
  children: [4, 304];
  seed_parents: [2, 49, 52, 93, 106, 109, 115, 118, 120, 127, 149, 153, 155, 159, 164, 166, 179, 191, 199, 204, 207, 209, 210, 215, 220, 227, 229, 242, 244, 250, 252, 261, 263, 269, 271, 276, 278, 280, 283, 285, 292, 294, 296, 301, 303];
  canMatchZeroChars: true
}
4 {
  kind: OneOrMore;
  children: [5];
  seed_parents: [3];
  canMatchZeroChars: false
}
5 {
  kind: Choice;
  children: [6, 28, 30];
  seed_parents: [4];
  canMatchZeroChars: false
}
6 {
  kind: Identifier;
  children: [7];
  identifier: %comment;
  seed_parents: [5];
  canMatchZeroChars: false
}
7 {
  kind: Choice;
  children: [8, 19];
  seed_parents: [6];
  canMatchZeroChars: false
}
8 {
  kind: Identifier;
  children: [9];
  identifier: $single_line_comment;
  seed_parents: [7];
  canMatchZeroChars: false
}
9 {
  kind: Sequence;
  children: [10, 11];
  seed_parents: [8];
  canMatchZeroChars: false
}
10 {
  kind: CharsetLiteral;
  literal: "/", "/";
  seed_parents: [9];
  canMatchZeroChars: false
}
11 {
  kind: Identifier;
  children: [12];
  identifier: $single_line_comment_rhs;
  seed_parents: [];
  canMatchZeroChars: false
}
12 {
  kind: Choice;
  children: [13, 15];
  seed_parents: [11, 18];
  canMatchZeroChars: false
}
13 {
  kind: Identifier;
  children: [14];
  identifier: $newline;
  seed_parents: [12];
  canMatchZeroChars: false
}
14 {
  kind: CharsetLiteral;
  literal: 10#10;
  seed_parents: [13, 30];
  canMatchZeroChars: false
}
15 {
  kind: Sequence;
  children: [16, 18];
  seed_parents: [12];
  canMatchZeroChars: false
}
16 {
  kind: Identifier;
  children: [17];
  identifier: $any;
  seed_parents: [15];
  canMatchZeroChars: false
}
17 {
  kind: CharsetLiteral;
  literal: [10#0..10#127];
  seed_parents: [16, 26];
  canMatchZeroChars: false
}
18 {
  kind: Identifier;
  children: [12];
  identifier: $single_line_comment_rhs;
  seed_parents: [];
  canMatchZeroChars: false
}
19 {
  kind: Identifier;
  children: [20];
  identifier: $multi_line_comment;
  seed_parents: [7];
  canMatchZeroChars: false
}
20 {
  kind: Sequence;
  children: [21, 22];
  seed_parents: [19];
  canMatchZeroChars: false
}
21 {
  kind: CharsetLiteral;
  literal: "/", "*";
  seed_parents: [20];
  canMatchZeroChars: false
}
22 {
  kind: Identifier;
  children: [23];
  identifier: $multi_line_comment_rhs;
  seed_parents: [];
  canMatchZeroChars: false
}
23 {
  kind: Choice;
  children: [24, 25];
  seed_parents: [22, 27];
  canMatchZeroChars: false
}
24 {
  kind: CharsetLiteral;
  literal: "*", "/";
  seed_parents: [23];
  canMatchZeroChars: false
}
25 {
  kind: Sequence;
  children: [26, 27];
  seed_parents: [23];
  canMatchZeroChars: false
}
26 {
  kind: Identifier;
  children: [17];
  identifier: $any;
  seed_parents: [25];
  canMatchZeroChars: false
}
27 {
  kind: Identifier;
  children: [23];
  identifier: $multi_line_comment_rhs;
  seed_parents: [];
  canMatchZeroChars: false
}
28 {
  kind: Identifier;
  children: [29];
  identifier: $whitespace;
  seed_parents: [5];
  canMatchZeroChars: false
}
29 {
  kind: CharsetLiteral;
  literal: " ";
  seed_parents: [28];
  canMatchZeroChars: false
}
30 {
  kind: Identifier;
  children: [14];
  identifier: $newline;
  seed_parents: [5];
  canMatchZeroChars: false
}
31 {
  kind: Identifier;
  children: [32];
  identifier: %statement;
  seed_parents: [1];
  canMatchZeroChars: false
}
32 {
  kind: Choice;
  children: [33, 288];
  seed_parents: [31, 302];
  canMatchZeroChars: false
}
33 {
  kind: Identifier;
  children: [34];
  identifier: %production;
  seed_parents: [32];
  canMatchZeroChars: false
}
34 {
  kind: Choice;
  children: [35, 213];
  seed_parents: [33];
  canMatchZeroChars: false
}
35 {
  kind: Sequence;
  children: [36, 49, 50, 52, 53, 210, 211];
  seed_parents: [34];
  canMatchZeroChars: false
}
36 {
  kind: Identifier;
  children: [37];
  identifier: %identifier;
  seed_parents: [35];
  canMatchZeroChars: false
}
37 {
  kind: Sequence;
  children: [38, 43];
  seed_parents: [36, 63, 135, 185, 214, 295];
  canMatchZeroChars: false
}
38 {
  kind: Choice;
  children: [39, 41];
  seed_parents: [37];
  canMatchZeroChars: false
}
39 {
  kind: Hidden;
  children: [40];
  seed_parents: [38];
  canMatchZeroChars: false
}
40 {
  kind: CharsetLiteral;
  literal: "$";
  seed_parents: [39];
  canMatchZeroChars: false
}
41 {
  kind: Hidden;
  children: [42];
  seed_parents: [38];
  canMatchZeroChars: false
}
42 {
  kind: CharsetLiteral;
  literal: "%";
  seed_parents: [41];
  canMatchZeroChars: false
}
43 {
  kind: Identifier;
  children: [44];
  identifier: $identifier_rhs;
  seed_parents: [];
  canMatchZeroChars: false
}
44 {
  kind: Sequence;
  children: [45, 46];
  seed_parents: [43];
  canMatchZeroChars: false
}
45 {
  kind: CharsetLiteral;
  literal: ["A".."Z""_""a".."z"];
  seed_parents: [44, 234];
  canMatchZeroChars: false
}
46 {
  kind: Choice;
  children: [47, 304];
  seed_parents: [];
  canMatchZeroChars: true
}
47 {
  kind: OneOrMore;
  children: [48];
  seed_parents: [46];
  canMatchZeroChars: false
}
48 {
  kind: CharsetLiteral;
  literal: ["0".."9""A".."Z""_""a".."z"];
  seed_parents: [47, 236];
  canMatchZeroChars: false
}
49 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
50 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
51 {
  kind: CharsetLiteral;
  literal: "=";
  seed_parents: [50, 228, 243, 251, 262, 270, 284, 293];
  canMatchZeroChars: false
}
52 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
53 {
  kind: Identifier;
  children: [54];
  identifier: %definition;
  seed_parents: [];
  canMatchZeroChars: false
}
54 {
  kind: Identifier;
  children: [55];
  identifier: %choices;
  seed_parents: [53, 286];
  canMatchZeroChars: false
}
55 {
  kind: Sequence;
  children: [56, 201, 209];
  seed_parents: [54, 128, 180, 192];
  canMatchZeroChars: false
}
56 {
  kind: Identifier;
  children: [57];
  identifier: %sequence;
  seed_parents: [55];
  canMatchZeroChars: false
}
57 {
  kind: Sequence;
  children: [58, 196];
  seed_parents: [56, 208];
  canMatchZeroChars: false
}
58 {
  kind: Identifier;
  children: [59];
  identifier: %expression;
  seed_parents: [57];
  canMatchZeroChars: false
}
59 {
  kind: Choice;
  children: [60, 132, 183];
  seed_parents: [58, 200];
  canMatchZeroChars: false
}
60 {
  kind: Identifier;
  children: [61];
  identifier: %hidden;
  seed_parents: [59];
  canMatchZeroChars: false
}
61 {
  kind: Choice;
  children: [62, 66, 77, 88, 124];
  seed_parents: [60];
  canMatchZeroChars: false
}
62 {
  kind: Sequence;
  children: [63, 64];
  seed_parents: [61];
  canMatchZeroChars: false
}
63 {
  kind: Identifier;
  children: [37];
  identifier: %identifier;
  seed_parents: [62];
  canMatchZeroChars: false
}
64 {
  kind: Identifier;
  children: [65];
  identifier: %tilde;
  seed_parents: [];
  canMatchZeroChars: false
}
65 {
  kind: CharsetLiteral;
  literal: "~";
  seed_parents: [64, 76, 87, 123, 131];
  canMatchZeroChars: false
}
66 {
  kind: Sequence;
  children: [67, 76];
  seed_parents: [61];
  canMatchZeroChars: false
}
67 {
  kind: Identifier;
  children: [68];
  identifier: %string_literal;
  seed_parents: [66];
  canMatchZeroChars: false
}
68 {
  kind: Sequence;
  children: [69, 71, 75];
  seed_parents: [67, 169, 186, 245];
  canMatchZeroChars: false
}
69 {
  kind: Hidden;
  children: [70];
  seed_parents: [68];
  canMatchZeroChars: false
}
70 {
  kind: CharsetLiteral;
  literal: 10#34;
  seed_parents: [69, 75, 102, 104, 232, 237];
  canMatchZeroChars: false
}
71 {
  kind: Choice;
  children: [72, 304];
  seed_parents: [];
  canMatchZeroChars: true
}
72 {
  kind: OneOrMore;
  children: [73];
  seed_parents: [71];
  canMatchZeroChars: false
}
73 {
  kind: Identifier;
  children: [74];
  identifier: $string_middle;
  seed_parents: [72];
  canMatchZeroChars: false
}
74 {
  kind: CharsetLiteral;
  literal: [" ".."!""#".."~"];
  seed_parents: [73, 103];
  canMatchZeroChars: false
}
75 {
  kind: Hidden;
  children: [70];
  seed_parents: [];
  canMatchZeroChars: false
}
76 {
  kind: Identifier;
  children: [65];
  identifier: %tilde;
  seed_parents: [];
  canMatchZeroChars: false
}
77 {
  kind: Sequence;
  children: [78, 87];
  seed_parents: [61];
  canMatchZeroChars: false
}
78 {
  kind: Identifier;
  children: [79];
  identifier: %integer_literal;
  seed_parents: [77];
  canMatchZeroChars: false
}
79 {
  kind: Sequence;
  children: [80, 83, 84];
  seed_parents: [78, 105, 152, 158, 165, 172, 187];
  canMatchZeroChars: false
}
80 {
  kind: Identifier;
  children: [81];
  identifier: $integer_literal_lhs;
  seed_parents: [79];
  canMatchZeroChars: false
}
81 {
  kind: OneOrMore;
  children: [82];
  seed_parents: [80];
  canMatchZeroChars: false
}
82 {
  kind: CharsetLiteral;
  literal: ["0".."9"];
  seed_parents: [81];
  canMatchZeroChars: false
}
83 {
  kind: CharsetLiteral;
  literal: "#";
  seed_parents: [];
  canMatchZeroChars: false
}
84 {
  kind: Identifier;
  children: [85];
  identifier: $integer_literal_rhs;
  seed_parents: [];
  canMatchZeroChars: false
}
85 {
  kind: OneOrMore;
  children: [86];
  seed_parents: [84];
  canMatchZeroChars: false
}
86 {
  kind: CharsetLiteral;
  literal: ["0".."9""A".."Z""a".."z"];
  seed_parents: [85];
  canMatchZeroChars: false
}
87 {
  kind: Identifier;
  children: [65];
  identifier: %tilde;
  seed_parents: [];
  canMatchZeroChars: false
}
88 {
  kind: Sequence;
  children: [89, 123];
  seed_parents: [61];
  canMatchZeroChars: false
}
89 {
  kind: Identifier;
  children: [90];
  identifier: %charset;
  seed_parents: [88];
  canMatchZeroChars: false
}
90 {
  kind: Sequence;
  children: [91, 93, 94, 112, 120, 121];
  seed_parents: [89, 175, 188];
  canMatchZeroChars: false
}
91 {
  kind: Hidden;
  children: [92];
  seed_parents: [90];
  canMatchZeroChars: false
}
92 {
  kind: CharsetLiteral;
  literal: "[";
  seed_parents: [91];
  canMatchZeroChars: false
}
93 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
94 {
  kind: Identifier;
  children: [95];
  identifier: %charset_item;
  seed_parents: [];
  canMatchZeroChars: false
}
95 {
  kind: Choice;
  children: [96, 111];
  seed_parents: [94, 119];
  canMatchZeroChars: false
}
96 {
  kind: Identifier;
  children: [97];
  identifier: %charset_range;
  seed_parents: [95];
  canMatchZeroChars: false
}
97 {
  kind: Sequence;
  children: [98, 106, 107, 109, 110];
  seed_parents: [96];
  canMatchZeroChars: false
}
98 {
  kind: Identifier;
  children: [99];
  identifier: %charset_literal;
  seed_parents: [97];
  canMatchZeroChars: false
}
99 {
  kind: Choice;
  children: [100, 105];
  seed_parents: [98, 110, 111];
  canMatchZeroChars: false
}
100 {
  kind: Identifier;
  children: [101];
  identifier: %character_literal;
  seed_parents: [99];
  canMatchZeroChars: false
}
101 {
  kind: Sequence;
  children: [102, 103, 104];
  seed_parents: [100];
  canMatchZeroChars: false
}
102 {
  kind: Hidden;
  children: [70];
  seed_parents: [101];
  canMatchZeroChars: false
}
103 {
  kind: Identifier;
  children: [74];
  identifier: $string_middle;
  seed_parents: [];
  canMatchZeroChars: false
}
104 {
  kind: Hidden;
  children: [70];
  seed_parents: [];
  canMatchZeroChars: false
}
105 {
  kind: Identifier;
  children: [79];
  identifier: %integer_literal;
  seed_parents: [99];
  canMatchZeroChars: false
}
106 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
107 {
  kind: Hidden;
  children: [108];
  seed_parents: [];
  canMatchZeroChars: false
}
108 {
  kind: CharsetLiteral;
  literal: ".", ".";
  seed_parents: [107];
  canMatchZeroChars: false
}
109 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
110 {
  kind: Identifier;
  children: [99];
  identifier: %charset_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
111 {
  kind: Identifier;
  children: [99];
  identifier: %charset_literal;
  seed_parents: [95];
  canMatchZeroChars: false
}
112 {
  kind: Choice;
  children: [113, 304];
  seed_parents: [];
  canMatchZeroChars: true
}
113 {
  kind: OneOrMore;
  children: [114];
  seed_parents: [112];
  canMatchZeroChars: false
}
114 {
  kind: Sequence;
  children: [115, 116, 118, 119];
  seed_parents: [113];
  canMatchZeroChars: false
}
115 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [114];
  canMatchZeroChars: true
}
116 {
  kind: Hidden;
  children: [117];
  seed_parents: [114];
  canMatchZeroChars: false
}
117 {
  kind: CharsetLiteral;
  literal: ",";
  seed_parents: [116, 154, 277];
  canMatchZeroChars: false
}
118 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
119 {
  kind: Identifier;
  children: [95];
  identifier: %charset_item;
  seed_parents: [];
  canMatchZeroChars: false
}
120 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
121 {
  kind: Hidden;
  children: [122];
  seed_parents: [];
  canMatchZeroChars: false
}
122 {
  kind: CharsetLiteral;
  literal: "]";
  seed_parents: [121];
  canMatchZeroChars: false
}
123 {
  kind: Identifier;
  children: [65];
  identifier: %tilde;
  seed_parents: [];
  canMatchZeroChars: false
}
124 {
  kind: Sequence;
  children: [125, 127, 128, 129, 131];
  seed_parents: [61];
  canMatchZeroChars: false
}
125 {
  kind: Hidden;
  children: [126];
  seed_parents: [124];
  canMatchZeroChars: false
}
126 {
  kind: CharsetLiteral;
  literal: "(";
  seed_parents: [125, 178, 190];
  canMatchZeroChars: false
}
127 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
128 {
  kind: Identifier;
  children: [55];
  identifier: %choices;
  seed_parents: [];
  canMatchZeroChars: false
}
129 {
  kind: Hidden;
  children: [130];
  seed_parents: [];
  canMatchZeroChars: false
}
130 {
  kind: CharsetLiteral;
  literal: ")";
  seed_parents: [129, 181, 193];
  canMatchZeroChars: false
}
131 {
  kind: Identifier;
  children: [65];
  identifier: %tilde;
  seed_parents: [];
  canMatchZeroChars: false
}
132 {
  kind: Identifier;
  children: [133];
  identifier: %repetition;
  seed_parents: [59];
  canMatchZeroChars: false
}
133 {
  kind: Choice;
  children: [134, 168, 171, 174, 177];
  seed_parents: [132];
  canMatchZeroChars: false
}
134 {
  kind: Sequence;
  children: [135, 136];
  seed_parents: [133];
  canMatchZeroChars: false
}
135 {
  kind: Identifier;
  children: [37];
  identifier: %identifier;
  seed_parents: [134];
  canMatchZeroChars: false
}
136 {
  kind: Identifier;
  children: [137];
  identifier: %repetition_postfix;
  seed_parents: [];
  canMatchZeroChars: false
}
137 {
  kind: Choice;
  children: [138, 140, 142, 144];
  seed_parents: [136, 170, 173, 176, 182];
  canMatchZeroChars: false
}
138 {
  kind: Identifier;
  children: [139];
  identifier: %question;
  seed_parents: [137];
  canMatchZeroChars: false
}
139 {
  kind: CharsetLiteral;
  literal: "?";
  seed_parents: [138];
  canMatchZeroChars: false
}
140 {
  kind: Identifier;
  children: [141];
  identifier: %star;
  seed_parents: [137];
  canMatchZeroChars: false
}
141 {
  kind: CharsetLiteral;
  literal: "*";
  seed_parents: [140];
  canMatchZeroChars: false
}
142 {
  kind: Identifier;
  children: [143];
  identifier: %plus;
  seed_parents: [137];
  canMatchZeroChars: false
}
143 {
  kind: CharsetLiteral;
  literal: "+";
  seed_parents: [142];
  canMatchZeroChars: false
}
144 {
  kind: Identifier;
  children: [145];
  identifier: %repetition_range;
  seed_parents: [137];
  canMatchZeroChars: false
}
145 {
  kind: Choice;
  children: [146, 162];
  seed_parents: [144];
  canMatchZeroChars: false
}
146 {
  kind: Sequence;
  children: [147, 149, 150, 154, 155, 156, 160];
  seed_parents: [145];
  canMatchZeroChars: false
}
147 {
  kind: Hidden;
  children: [148];
  seed_parents: [146];
  canMatchZeroChars: false
}
148 {
  kind: CharsetLiteral;
  literal: "{";
  seed_parents: [147, 163];
  canMatchZeroChars: false
}
149 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
150 {
  kind: Choice;
  children: [151, 304];
  seed_parents: [];
  canMatchZeroChars: true
}
151 {
  kind: Sequence;
  children: [152, 153];
  seed_parents: [150];
  canMatchZeroChars: false
}
152 {
  kind: Identifier;
  children: [79];
  identifier: %integer_literal;
  seed_parents: [151];
  canMatchZeroChars: false
}
153 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
154 {
  kind: Identifier;
  children: [117];
  identifier: %comma;
  seed_parents: [];
  canMatchZeroChars: false
}
155 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
156 {
  kind: Choice;
  children: [157, 304];
  seed_parents: [];
  canMatchZeroChars: true
}
157 {
  kind: Sequence;
  children: [158, 159];
  seed_parents: [156];
  canMatchZeroChars: false
}
158 {
  kind: Identifier;
  children: [79];
  identifier: %integer_literal;
  seed_parents: [157];
  canMatchZeroChars: false
}
159 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
160 {
  kind: Hidden;
  children: [161];
  seed_parents: [];
  canMatchZeroChars: false
}
161 {
  kind: CharsetLiteral;
  literal: "}";
  seed_parents: [160, 167];
  canMatchZeroChars: false
}
162 {
  kind: Sequence;
  children: [163, 164, 165, 166, 167];
  seed_parents: [145];
  canMatchZeroChars: false
}
163 {
  kind: Hidden;
  children: [148];
  seed_parents: [162];
  canMatchZeroChars: false
}
164 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
165 {
  kind: Identifier;
  children: [79];
  identifier: %integer_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
166 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
167 {
  kind: Hidden;
  children: [161];
  seed_parents: [];
  canMatchZeroChars: false
}
168 {
  kind: Sequence;
  children: [169, 170];
  seed_parents: [133];
  canMatchZeroChars: false
}
169 {
  kind: Identifier;
  children: [68];
  identifier: %string_literal;
  seed_parents: [168];
  canMatchZeroChars: false
}
170 {
  kind: Identifier;
  children: [137];
  identifier: %repetition_postfix;
  seed_parents: [];
  canMatchZeroChars: false
}
171 {
  kind: Sequence;
  children: [172, 173];
  seed_parents: [133];
  canMatchZeroChars: false
}
172 {
  kind: Identifier;
  children: [79];
  identifier: %integer_literal;
  seed_parents: [171];
  canMatchZeroChars: false
}
173 {
  kind: Identifier;
  children: [137];
  identifier: %repetition_postfix;
  seed_parents: [];
  canMatchZeroChars: false
}
174 {
  kind: Sequence;
  children: [175, 176];
  seed_parents: [133];
  canMatchZeroChars: false
}
175 {
  kind: Identifier;
  children: [90];
  identifier: %charset;
  seed_parents: [174];
  canMatchZeroChars: false
}
176 {
  kind: Identifier;
  children: [137];
  identifier: %repetition_postfix;
  seed_parents: [];
  canMatchZeroChars: false
}
177 {
  kind: Sequence;
  children: [178, 179, 180, 181, 182];
  seed_parents: [133];
  canMatchZeroChars: false
}
178 {
  kind: Hidden;
  children: [126];
  seed_parents: [177];
  canMatchZeroChars: false
}
179 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
180 {
  kind: Identifier;
  children: [55];
  identifier: %choices;
  seed_parents: [];
  canMatchZeroChars: false
}
181 {
  kind: Hidden;
  children: [130];
  seed_parents: [];
  canMatchZeroChars: false
}
182 {
  kind: Identifier;
  children: [137];
  identifier: %repetition_postfix;
  seed_parents: [];
  canMatchZeroChars: false
}
183 {
  kind: Identifier;
  children: [184];
  identifier: %plain;
  seed_parents: [59];
  canMatchZeroChars: false
}
184 {
  kind: Choice;
  children: [185, 186, 187, 188, 189, 194];
  seed_parents: [183];
  canMatchZeroChars: false
}
185 {
  kind: Identifier;
  children: [37];
  identifier: %identifier;
  seed_parents: [184];
  canMatchZeroChars: false
}
186 {
  kind: Identifier;
  children: [68];
  identifier: %string_literal;
  seed_parents: [184];
  canMatchZeroChars: false
}
187 {
  kind: Identifier;
  children: [79];
  identifier: %integer_literal;
  seed_parents: [184];
  canMatchZeroChars: false
}
188 {
  kind: Identifier;
  children: [90];
  identifier: %charset;
  seed_parents: [184];
  canMatchZeroChars: false
}
189 {
  kind: Sequence;
  children: [190, 191, 192, 193];
  seed_parents: [184];
  canMatchZeroChars: false
}
190 {
  kind: Hidden;
  children: [126];
  seed_parents: [189];
  canMatchZeroChars: false
}
191 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
192 {
  kind: Identifier;
  children: [55];
  identifier: %choices;
  seed_parents: [];
  canMatchZeroChars: false
}
193 {
  kind: Hidden;
  children: [130];
  seed_parents: [];
  canMatchZeroChars: false
}
194 {
  kind: Identifier;
  children: [195];
  identifier: %kw_epsilon;
  seed_parents: [184];
  canMatchZeroChars: false
}
195 {
  kind: CharsetLiteral;
  literal: "e", "p", "s", "i", "l", "o", "n";
  seed_parents: [194];
  canMatchZeroChars: false
}
196 {
  kind: Choice;
  children: [197, 304];
  seed_parents: [];
  canMatchZeroChars: true
}
197 {
  kind: OneOrMore;
  children: [198];
  seed_parents: [196];
  canMatchZeroChars: false
}
198 {
  kind: Sequence;
  children: [199, 200];
  seed_parents: [197];
  canMatchZeroChars: false
}
199 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [198];
  canMatchZeroChars: true
}
200 {
  kind: Identifier;
  children: [59];
  identifier: %expression;
  seed_parents: [198];
  canMatchZeroChars: false
}
201 {
  kind: Choice;
  children: [202, 304];
  seed_parents: [];
  canMatchZeroChars: true
}
202 {
  kind: OneOrMore;
  children: [203];
  seed_parents: [201];
  canMatchZeroChars: false
}
203 {
  kind: Sequence;
  children: [204, 205, 207, 208];
  seed_parents: [202];
  canMatchZeroChars: false
}
204 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [203];
  canMatchZeroChars: true
}
205 {
  kind: Hidden;
  children: [206];
  seed_parents: [203];
  canMatchZeroChars: false
}
206 {
  kind: CharsetLiteral;
  literal: "|";
  seed_parents: [205];
  canMatchZeroChars: false
}
207 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
208 {
  kind: Identifier;
  children: [57];
  identifier: %sequence;
  seed_parents: [];
  canMatchZeroChars: false
}
209 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
210 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
211 {
  kind: Hidden;
  children: [212];
  seed_parents: [];
  canMatchZeroChars: false
}
212 {
  kind: CharsetLiteral;
  literal: ";";
  seed_parents: [211, 287, 297];
  canMatchZeroChars: false
}
213 {
  kind: Sequence;
  children: [214, 215, 216, 218, 281, 283, 284, 285, 286, 287];
  seed_parents: [34];
  canMatchZeroChars: false
}
214 {
  kind: Identifier;
  children: [37];
  identifier: %identifier;
  seed_parents: [213];
  canMatchZeroChars: false
}
215 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
216 {
  kind: Hidden;
  children: [217];
  seed_parents: [];
  canMatchZeroChars: false
}
217 {
  kind: CharsetLiteral;
  literal: 10#60;
  seed_parents: [216];
  canMatchZeroChars: false
}
218 {
  kind: Identifier;
  children: [219];
  identifier: %parameter_list;
  seed_parents: [];
  canMatchZeroChars: false
}
219 {
  kind: Sequence;
  children: [220, 221, 273, 280];
  seed_parents: [218];
  canMatchZeroChars: false
}
220 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [219];
  canMatchZeroChars: true
}
221 {
  kind: Identifier;
  children: [222];
  identifier: %parameter;
  seed_parents: [219];
  canMatchZeroChars: false
}
222 {
  kind: Choice;
  children: [223, 238, 246, 257, 265];
  seed_parents: [221, 279];
  canMatchZeroChars: false
}
223 {
  kind: Identifier;
  children: [224];
  identifier: %parameter_id;
  seed_parents: [222];
  canMatchZeroChars: false
}
224 {
  kind: Sequence;
  children: [225, 227, 228, 229, 230];
  seed_parents: [223];
  canMatchZeroChars: false
}
225 {
  kind: Identifier;
  children: [226];
  identifier: %kw_parameter_id;
  seed_parents: [224];
  canMatchZeroChars: false
}
226 {
  kind: CharsetLiteral;
  literal: "i", "d";
  seed_parents: [225];
  canMatchZeroChars: false
}
227 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
228 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
229 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
230 {
  kind: Identifier;
  children: [231];
  identifier: %id_string_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
231 {
  kind: Sequence;
  children: [232, 233, 237];
  seed_parents: [230];
  canMatchZeroChars: false
}
232 {
  kind: Hidden;
  children: [70];
  seed_parents: [231];
  canMatchZeroChars: false
}
233 {
  kind: Identifier;
  children: [234];
  identifier: $id_string_middle;
  seed_parents: [];
  canMatchZeroChars: false
}
234 {
  kind: Sequence;
  children: [45, 235];
  seed_parents: [233];
  canMatchZeroChars: false
}
235 {
  kind: Choice;
  children: [236, 304];
  seed_parents: [];
  canMatchZeroChars: true
}
236 {
  kind: OneOrMore;
  children: [48];
  seed_parents: [235];
  canMatchZeroChars: false
}
237 {
  kind: Hidden;
  children: [70];
  seed_parents: [];
  canMatchZeroChars: false
}
238 {
  kind: Identifier;
  children: [239];
  identifier: %parameter_displayname;
  seed_parents: [222];
  canMatchZeroChars: false
}
239 {
  kind: Sequence;
  children: [240, 242, 243, 244, 245];
  seed_parents: [238];
  canMatchZeroChars: false
}
240 {
  kind: Identifier;
  children: [241];
  identifier: %kw_parameter_display_name;
  seed_parents: [239];
  canMatchZeroChars: false
}
241 {
  kind: CharsetLiteral;
  literal: "d", "i", "s", "p", "l", "a", "y", "_", "n", "a", "m", "e";
  seed_parents: [240];
  canMatchZeroChars: false
}
242 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
243 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
244 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
245 {
  kind: Identifier;
  children: [68];
  identifier: %string_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
246 {
  kind: Identifier;
  children: [247];
  identifier: %parameter_unpack;
  seed_parents: [222];
  canMatchZeroChars: false
}
247 {
  kind: Sequence;
  children: [248, 250, 251, 252, 253];
  seed_parents: [246];
  canMatchZeroChars: false
}
248 {
  kind: Identifier;
  children: [249];
  identifier: %kw_parameter_unpack;
  seed_parents: [247];
  canMatchZeroChars: false
}
249 {
  kind: CharsetLiteral;
  literal: "u", "n", "p", "a", "c", "k";
  seed_parents: [248];
  canMatchZeroChars: false
}
250 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
251 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
252 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
253 {
  kind: Identifier;
  children: [254];
  identifier: %boolean_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
254 {
  kind: Choice;
  children: [255, 256];
  seed_parents: [253, 264, 272];
  canMatchZeroChars: false
}
255 {
  kind: CharsetLiteral;
  literal: "t", "r", "u", "e";
  seed_parents: [254];
  canMatchZeroChars: false
}
256 {
  kind: CharsetLiteral;
  literal: "f", "a", "l", "s", "e";
  seed_parents: [254];
  canMatchZeroChars: false
}
257 {
  kind: Identifier;
  children: [258];
  identifier: %parameter_hide;
  seed_parents: [222];
  canMatchZeroChars: false
}
258 {
  kind: Sequence;
  children: [259, 261, 262, 263, 264];
  seed_parents: [257];
  canMatchZeroChars: false
}
259 {
  kind: Identifier;
  children: [260];
  identifier: %kw_parameter_hide;
  seed_parents: [258];
  canMatchZeroChars: false
}
260 {
  kind: CharsetLiteral;
  literal: "h", "i", "d", "e";
  seed_parents: [259];
  canMatchZeroChars: false
}
261 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
262 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
263 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
264 {
  kind: Identifier;
  children: [254];
  identifier: %boolean_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
265 {
  kind: Identifier;
  children: [266];
  identifier: %parameter_merge;
  seed_parents: [222];
  canMatchZeroChars: false
}
266 {
  kind: Sequence;
  children: [267, 269, 270, 271, 272];
  seed_parents: [265];
  canMatchZeroChars: false
}
267 {
  kind: Identifier;
  children: [268];
  identifier: %kw_parameter_merge;
  seed_parents: [266];
  canMatchZeroChars: false
}
268 {
  kind: CharsetLiteral;
  literal: "m", "e", "r", "g", "e";
  seed_parents: [267];
  canMatchZeroChars: false
}
269 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
270 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
271 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
272 {
  kind: Identifier;
  children: [254];
  identifier: %boolean_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
273 {
  kind: Choice;
  children: [274, 304];
  seed_parents: [];
  canMatchZeroChars: true
}
274 {
  kind: OneOrMore;
  children: [275];
  seed_parents: [273];
  canMatchZeroChars: false
}
275 {
  kind: Sequence;
  children: [276, 277, 278, 279];
  seed_parents: [274];
  canMatchZeroChars: false
}
276 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [275];
  canMatchZeroChars: true
}
277 {
  kind: Hidden;
  children: [117];
  seed_parents: [275];
  canMatchZeroChars: false
}
278 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
279 {
  kind: Identifier;
  children: [222];
  identifier: %parameter;
  seed_parents: [];
  canMatchZeroChars: false
}
280 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
281 {
  kind: Hidden;
  children: [282];
  seed_parents: [];
  canMatchZeroChars: false
}
282 {
  kind: CharsetLiteral;
  literal: 10#62;
  seed_parents: [281];
  canMatchZeroChars: false
}
283 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
284 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
285 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
286 {
  kind: Identifier;
  children: [54];
  identifier: %definition;
  seed_parents: [];
  canMatchZeroChars: false
}
287 {
  kind: Hidden;
  children: [212];
  seed_parents: [];
  canMatchZeroChars: false
}
288 {
  kind: Identifier;
  children: [289];
  identifier: %grammar_property;
  seed_parents: [32];
  canMatchZeroChars: false
}
289 {
  kind: Sequence;
  children: [290, 292, 293, 294, 295, 296, 297];
  seed_parents: [288];
  canMatchZeroChars: false
}
290 {
  kind: Identifier;
  children: [291];
  identifier: %grammar_property_start;
  seed_parents: [289];
  canMatchZeroChars: false
}
291 {
  kind: CharsetLiteral;
  literal: "@", "s", "t", "a", "r", "t";
  seed_parents: [290];
  canMatchZeroChars: false
}
292 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
293 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
294 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
295 {
  kind: Identifier;
  children: [37];
  identifier: %identifier;
  seed_parents: [];
  canMatchZeroChars: false
}
296 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
297 {
  kind: Hidden;
  children: [212];
  seed_parents: [];
  canMatchZeroChars: false
}
298 {
  kind: Choice;
  children: [299, 304];
  seed_parents: [];
  canMatchZeroChars: true
}
299 {
  kind: OneOrMore;
  children: [300];
  seed_parents: [298];
  canMatchZeroChars: false
}
300 {
  kind: Sequence;
  children: [301, 302];
  seed_parents: [299];
  canMatchZeroChars: false
}
301 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [300];
  canMatchZeroChars: true
}
302 {
  kind: Identifier;
  children: [32];
  identifier: %statement;
  seed_parents: [300];
  canMatchZeroChars: false
}
303 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
304 {
  kind: Epsilon;
  children: [];
  seed_parents: [3, 46, 71, 112, 150, 156, 196, 201, 235, 273, 298];
  canMatchZeroChars: true
}

