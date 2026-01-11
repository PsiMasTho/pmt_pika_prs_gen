0 {
  kind: Identifier;
  children: [1];
  identifier: %grammar;
  seed_parents: [];
  canMatchZeroChars: false
}
1 {
  kind: Sequence;
  children: [2, 31, 278, 283];
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
  children: [4, 284];
  seed_parents: [2, 49, 52, 76, 88, 91, 95, 100, 102, 135, 148, 151, 157, 159, 161, 170, 179, 184, 187, 189, 190, 195, 200, 207, 209, 222, 224, 230, 232, 241, 243, 249, 251, 256, 258, 260, 263, 265, 272, 274, 276, 281, 283];
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
  children: [33, 268];
  seed_parents: [31, 282];
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
  children: [35, 193];
  seed_parents: [33];
  canMatchZeroChars: false
}
35 {
  kind: Sequence;
  children: [36, 49, 50, 52, 53, 190, 191];
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
  seed_parents: [36, 61, 105, 194, 275];
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
  seed_parents: [44, 214];
  canMatchZeroChars: false
}
46 {
  kind: Choice;
  children: [47, 284];
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
  seed_parents: [47, 216];
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
  seed_parents: [50, 208, 223, 231, 242, 250, 264, 273];
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
  seed_parents: [53, 266];
  canMatchZeroChars: false
}
55 {
  kind: Sequence;
  children: [56, 181, 189];
  seed_parents: [54, 171];
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
  children: [58, 176];
  seed_parents: [56, 188];
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
  children: [60, 104, 110, 123, 128, 130, 167];
  seed_parents: [58, 180];
  canMatchZeroChars: false
}
60 {
  kind: Sequence;
  children: [61, 62];
  seed_parents: [59];
  canMatchZeroChars: false
}
61 {
  kind: Identifier;
  children: [37];
  identifier: %identifier;
  seed_parents: [60];
  canMatchZeroChars: false
}
62 {
  kind: Choice;
  children: [63, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
63 {
  kind: Identifier;
  children: [64];
  identifier: %repetition_expression;
  seed_parents: [62];
  canMatchZeroChars: false
}
64 {
  kind: Choice;
  children: [65, 67, 69, 71];
  seed_parents: [63, 107, 121, 126, 165, 175];
  canMatchZeroChars: false
}
65 {
  kind: Identifier;
  children: [66];
  identifier: %question;
  seed_parents: [64];
  canMatchZeroChars: false
}
66 {
  kind: CharsetLiteral;
  literal: "?";
  seed_parents: [65];
  canMatchZeroChars: false
}
67 {
  kind: Identifier;
  children: [68];
  identifier: %star;
  seed_parents: [64];
  canMatchZeroChars: false
}
68 {
  kind: CharsetLiteral;
  literal: "*";
  seed_parents: [67];
  canMatchZeroChars: false
}
69 {
  kind: Identifier;
  children: [70];
  identifier: %plus;
  seed_parents: [64];
  canMatchZeroChars: false
}
70 {
  kind: CharsetLiteral;
  literal: "+";
  seed_parents: [69];
  canMatchZeroChars: false
}
71 {
  kind: Identifier;
  children: [72];
  identifier: %repetition_range;
  seed_parents: [64];
  canMatchZeroChars: false
}
72 {
  kind: Choice;
  children: [73, 98];
  seed_parents: [71];
  canMatchZeroChars: false
}
73 {
  kind: Sequence;
  children: [74, 76, 77, 89, 91, 92, 96];
  seed_parents: [72];
  canMatchZeroChars: false
}
74 {
  kind: Hidden;
  children: [75];
  seed_parents: [73];
  canMatchZeroChars: false
}
75 {
  kind: CharsetLiteral;
  literal: "{";
  seed_parents: [74, 99];
  canMatchZeroChars: false
}
76 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
77 {
  kind: Choice;
  children: [78, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
78 {
  kind: Sequence;
  children: [79, 88];
  seed_parents: [77];
  canMatchZeroChars: false
}
79 {
  kind: Identifier;
  children: [80];
  identifier: %integer_literal;
  seed_parents: [78];
  canMatchZeroChars: false
}
80 {
  kind: Sequence;
  children: [81, 84, 85];
  seed_parents: [79, 94, 101, 124, 147];
  canMatchZeroChars: false
}
81 {
  kind: Identifier;
  children: [82];
  identifier: $integer_literal_lhs;
  seed_parents: [80];
  canMatchZeroChars: false
}
82 {
  kind: OneOrMore;
  children: [83];
  seed_parents: [81];
  canMatchZeroChars: false
}
83 {
  kind: CharsetLiteral;
  literal: ["0".."9"];
  seed_parents: [82];
  canMatchZeroChars: false
}
84 {
  kind: CharsetLiteral;
  literal: "#";
  seed_parents: [];
  canMatchZeroChars: false
}
85 {
  kind: Identifier;
  children: [86];
  identifier: $integer_literal_rhs;
  seed_parents: [];
  canMatchZeroChars: false
}
86 {
  kind: OneOrMore;
  children: [87];
  seed_parents: [85];
  canMatchZeroChars: false
}
87 {
  kind: CharsetLiteral;
  literal: ["0".."9""A".."Z""a".."z"];
  seed_parents: [86];
  canMatchZeroChars: false
}
88 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
89 {
  kind: Identifier;
  children: [90];
  identifier: %comma;
  seed_parents: [];
  canMatchZeroChars: false
}
90 {
  kind: CharsetLiteral;
  literal: ",";
  seed_parents: [89, 158, 257];
  canMatchZeroChars: false
}
91 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
92 {
  kind: Choice;
  children: [93, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
93 {
  kind: Sequence;
  children: [94, 95];
  seed_parents: [92];
  canMatchZeroChars: false
}
94 {
  kind: Identifier;
  children: [80];
  identifier: %integer_literal;
  seed_parents: [93];
  canMatchZeroChars: false
}
95 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
96 {
  kind: Hidden;
  children: [97];
  seed_parents: [];
  canMatchZeroChars: false
}
97 {
  kind: CharsetLiteral;
  literal: "}";
  seed_parents: [96, 103];
  canMatchZeroChars: false
}
98 {
  kind: Sequence;
  children: [99, 100, 101, 102, 103];
  seed_parents: [72];
  canMatchZeroChars: false
}
99 {
  kind: Hidden;
  children: [75];
  seed_parents: [98];
  canMatchZeroChars: false
}
100 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
101 {
  kind: Identifier;
  children: [80];
  identifier: %integer_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
102 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
103 {
  kind: Hidden;
  children: [97];
  seed_parents: [];
  canMatchZeroChars: false
}
104 {
  kind: Sequence;
  children: [105, 106];
  seed_parents: [59];
  canMatchZeroChars: false
}
105 {
  kind: Identifier;
  children: [37];
  identifier: %identifier;
  seed_parents: [104];
  canMatchZeroChars: false
}
106 {
  kind: Choice;
  children: [107, 108, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
107 {
  kind: Identifier;
  children: [64];
  identifier: %repetition_expression;
  seed_parents: [106];
  canMatchZeroChars: false
}
108 {
  kind: Identifier;
  children: [109];
  identifier: %tilde;
  seed_parents: [106];
  canMatchZeroChars: false
}
109 {
  kind: CharsetLiteral;
  literal: "~";
  seed_parents: [108, 122, 127, 166];
  canMatchZeroChars: false
}
110 {
  kind: Sequence;
  children: [111, 120];
  seed_parents: [59];
  canMatchZeroChars: false
}
111 {
  kind: Identifier;
  children: [112];
  identifier: %string_literal;
  seed_parents: [110];
  canMatchZeroChars: false
}
112 {
  kind: Sequence;
  children: [113, 115, 119];
  seed_parents: [111, 225];
  canMatchZeroChars: false
}
113 {
  kind: Hidden;
  children: [114];
  seed_parents: [112];
  canMatchZeroChars: false
}
114 {
  kind: CharsetLiteral;
  literal: 10#34;
  seed_parents: [113, 119, 144, 146, 212, 217];
  canMatchZeroChars: false
}
115 {
  kind: Choice;
  children: [116, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
116 {
  kind: OneOrMore;
  children: [117];
  seed_parents: [115];
  canMatchZeroChars: false
}
117 {
  kind: Identifier;
  children: [118];
  identifier: $string_middle;
  seed_parents: [116];
  canMatchZeroChars: false
}
118 {
  kind: CharsetLiteral;
  literal: [" ".."!""#".."~"];
  seed_parents: [117, 145];
  canMatchZeroChars: false
}
119 {
  kind: Hidden;
  children: [114];
  seed_parents: [];
  canMatchZeroChars: false
}
120 {
  kind: Choice;
  children: [121, 122, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
121 {
  kind: Identifier;
  children: [64];
  identifier: %repetition_expression;
  seed_parents: [120];
  canMatchZeroChars: false
}
122 {
  kind: Identifier;
  children: [109];
  identifier: %tilde;
  seed_parents: [120];
  canMatchZeroChars: false
}
123 {
  kind: Sequence;
  children: [124, 125];
  seed_parents: [59];
  canMatchZeroChars: false
}
124 {
  kind: Identifier;
  children: [80];
  identifier: %integer_literal;
  seed_parents: [123];
  canMatchZeroChars: false
}
125 {
  kind: Choice;
  children: [126, 127, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
126 {
  kind: Identifier;
  children: [64];
  identifier: %repetition_expression;
  seed_parents: [125];
  canMatchZeroChars: false
}
127 {
  kind: Identifier;
  children: [109];
  identifier: %tilde;
  seed_parents: [125];
  canMatchZeroChars: false
}
128 {
  kind: Identifier;
  children: [129];
  identifier: %kw_epsilon;
  seed_parents: [59];
  canMatchZeroChars: false
}
129 {
  kind: CharsetLiteral;
  literal: "e", "p", "s", "i", "l", "o", "n";
  seed_parents: [128];
  canMatchZeroChars: false
}
130 {
  kind: Sequence;
  children: [131, 164];
  seed_parents: [59];
  canMatchZeroChars: false
}
131 {
  kind: Identifier;
  children: [132];
  identifier: %charset_expression;
  seed_parents: [130];
  canMatchZeroChars: false
}
132 {
  kind: Sequence;
  children: [133, 135, 136, 154, 161, 162];
  seed_parents: [131];
  canMatchZeroChars: false
}
133 {
  kind: Hidden;
  children: [134];
  seed_parents: [132];
  canMatchZeroChars: false
}
134 {
  kind: CharsetLiteral;
  literal: "[";
  seed_parents: [133];
  canMatchZeroChars: false
}
135 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
136 {
  kind: Identifier;
  children: [137];
  identifier: %charset_item;
  seed_parents: [];
  canMatchZeroChars: false
}
137 {
  kind: Choice;
  children: [138, 153];
  seed_parents: [136, 160];
  canMatchZeroChars: false
}
138 {
  kind: Identifier;
  children: [139];
  identifier: %charset_range;
  seed_parents: [137];
  canMatchZeroChars: false
}
139 {
  kind: Sequence;
  children: [140, 148, 149, 151, 152];
  seed_parents: [138];
  canMatchZeroChars: false
}
140 {
  kind: Identifier;
  children: [141];
  identifier: %charset_literal;
  seed_parents: [139];
  canMatchZeroChars: false
}
141 {
  kind: Choice;
  children: [142, 147];
  seed_parents: [140, 152, 153];
  canMatchZeroChars: false
}
142 {
  kind: Identifier;
  children: [143];
  identifier: %character_literal;
  seed_parents: [141];
  canMatchZeroChars: false
}
143 {
  kind: Sequence;
  children: [144, 145, 146];
  seed_parents: [142];
  canMatchZeroChars: false
}
144 {
  kind: Hidden;
  children: [114];
  seed_parents: [143];
  canMatchZeroChars: false
}
145 {
  kind: Identifier;
  children: [118];
  identifier: $string_middle;
  seed_parents: [];
  canMatchZeroChars: false
}
146 {
  kind: Hidden;
  children: [114];
  seed_parents: [];
  canMatchZeroChars: false
}
147 {
  kind: Identifier;
  children: [80];
  identifier: %integer_literal;
  seed_parents: [141];
  canMatchZeroChars: false
}
148 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
149 {
  kind: Hidden;
  children: [150];
  seed_parents: [];
  canMatchZeroChars: false
}
150 {
  kind: CharsetLiteral;
  literal: ".", ".";
  seed_parents: [149];
  canMatchZeroChars: false
}
151 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
152 {
  kind: Identifier;
  children: [141];
  identifier: %charset_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
153 {
  kind: Identifier;
  children: [141];
  identifier: %charset_literal;
  seed_parents: [137];
  canMatchZeroChars: false
}
154 {
  kind: Choice;
  children: [155, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
155 {
  kind: OneOrMore;
  children: [156];
  seed_parents: [154];
  canMatchZeroChars: false
}
156 {
  kind: Sequence;
  children: [157, 158, 159, 160];
  seed_parents: [155];
  canMatchZeroChars: false
}
157 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [156];
  canMatchZeroChars: true
}
158 {
  kind: Hidden;
  children: [90];
  seed_parents: [156];
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
  kind: Identifier;
  children: [137];
  identifier: %charset_item;
  seed_parents: [];
  canMatchZeroChars: false
}
161 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
162 {
  kind: Hidden;
  children: [163];
  seed_parents: [];
  canMatchZeroChars: false
}
163 {
  kind: CharsetLiteral;
  literal: "]";
  seed_parents: [162];
  canMatchZeroChars: false
}
164 {
  kind: Choice;
  children: [165, 166, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
165 {
  kind: Identifier;
  children: [64];
  identifier: %repetition_expression;
  seed_parents: [164];
  canMatchZeroChars: false
}
166 {
  kind: Identifier;
  children: [109];
  identifier: %tilde;
  seed_parents: [164];
  canMatchZeroChars: false
}
167 {
  kind: Sequence;
  children: [168, 170, 171, 172, 174];
  seed_parents: [59];
  canMatchZeroChars: false
}
168 {
  kind: Hidden;
  children: [169];
  seed_parents: [167];
  canMatchZeroChars: false
}
169 {
  kind: CharsetLiteral;
  literal: "(";
  seed_parents: [168];
  canMatchZeroChars: false
}
170 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
171 {
  kind: Identifier;
  children: [55];
  identifier: %choices;
  seed_parents: [];
  canMatchZeroChars: false
}
172 {
  kind: Hidden;
  children: [173];
  seed_parents: [];
  canMatchZeroChars: false
}
173 {
  kind: CharsetLiteral;
  literal: ")";
  seed_parents: [172];
  canMatchZeroChars: false
}
174 {
  kind: Choice;
  children: [175, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
175 {
  kind: Identifier;
  children: [64];
  identifier: %repetition_expression;
  seed_parents: [174];
  canMatchZeroChars: false
}
176 {
  kind: Choice;
  children: [177, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
177 {
  kind: OneOrMore;
  children: [178];
  seed_parents: [176];
  canMatchZeroChars: false
}
178 {
  kind: Sequence;
  children: [179, 180];
  seed_parents: [177];
  canMatchZeroChars: false
}
179 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [178];
  canMatchZeroChars: true
}
180 {
  kind: Identifier;
  children: [59];
  identifier: %expression;
  seed_parents: [178];
  canMatchZeroChars: false
}
181 {
  kind: Choice;
  children: [182, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
182 {
  kind: OneOrMore;
  children: [183];
  seed_parents: [181];
  canMatchZeroChars: false
}
183 {
  kind: Sequence;
  children: [184, 185, 187, 188];
  seed_parents: [182];
  canMatchZeroChars: false
}
184 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [183];
  canMatchZeroChars: true
}
185 {
  kind: Hidden;
  children: [186];
  seed_parents: [183];
  canMatchZeroChars: false
}
186 {
  kind: CharsetLiteral;
  literal: "|";
  seed_parents: [185];
  canMatchZeroChars: false
}
187 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
188 {
  kind: Identifier;
  children: [57];
  identifier: %sequence;
  seed_parents: [];
  canMatchZeroChars: false
}
189 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
190 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
191 {
  kind: Hidden;
  children: [192];
  seed_parents: [];
  canMatchZeroChars: false
}
192 {
  kind: CharsetLiteral;
  literal: ";";
  seed_parents: [191, 267, 277];
  canMatchZeroChars: false
}
193 {
  kind: Sequence;
  children: [194, 195, 196, 198, 261, 263, 264, 265, 266, 267];
  seed_parents: [34];
  canMatchZeroChars: false
}
194 {
  kind: Identifier;
  children: [37];
  identifier: %identifier;
  seed_parents: [193];
  canMatchZeroChars: false
}
195 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
196 {
  kind: Hidden;
  children: [197];
  seed_parents: [];
  canMatchZeroChars: false
}
197 {
  kind: CharsetLiteral;
  literal: 10#60;
  seed_parents: [196];
  canMatchZeroChars: false
}
198 {
  kind: Identifier;
  children: [199];
  identifier: %parameter_list;
  seed_parents: [];
  canMatchZeroChars: false
}
199 {
  kind: Sequence;
  children: [200, 201, 253, 260];
  seed_parents: [198];
  canMatchZeroChars: false
}
200 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [199];
  canMatchZeroChars: true
}
201 {
  kind: Identifier;
  children: [202];
  identifier: %parameter;
  seed_parents: [199];
  canMatchZeroChars: false
}
202 {
  kind: Choice;
  children: [203, 218, 226, 237, 245];
  seed_parents: [201, 259];
  canMatchZeroChars: false
}
203 {
  kind: Identifier;
  children: [204];
  identifier: %parameter_id;
  seed_parents: [202];
  canMatchZeroChars: false
}
204 {
  kind: Sequence;
  children: [205, 207, 208, 209, 210];
  seed_parents: [203];
  canMatchZeroChars: false
}
205 {
  kind: Identifier;
  children: [206];
  identifier: %kw_parameter_id;
  seed_parents: [204];
  canMatchZeroChars: false
}
206 {
  kind: CharsetLiteral;
  literal: "i", "d";
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
  kind: Hidden;
  children: [51];
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
  children: [211];
  identifier: %id_string_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
211 {
  kind: Sequence;
  children: [212, 213, 217];
  seed_parents: [210];
  canMatchZeroChars: false
}
212 {
  kind: Hidden;
  children: [114];
  seed_parents: [211];
  canMatchZeroChars: false
}
213 {
  kind: Identifier;
  children: [214];
  identifier: $id_string_middle;
  seed_parents: [];
  canMatchZeroChars: false
}
214 {
  kind: Sequence;
  children: [45, 215];
  seed_parents: [213];
  canMatchZeroChars: false
}
215 {
  kind: Choice;
  children: [216, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
216 {
  kind: OneOrMore;
  children: [48];
  seed_parents: [215];
  canMatchZeroChars: false
}
217 {
  kind: Hidden;
  children: [114];
  seed_parents: [];
  canMatchZeroChars: false
}
218 {
  kind: Identifier;
  children: [219];
  identifier: %parameter_displayname;
  seed_parents: [202];
  canMatchZeroChars: false
}
219 {
  kind: Sequence;
  children: [220, 222, 223, 224, 225];
  seed_parents: [218];
  canMatchZeroChars: false
}
220 {
  kind: Identifier;
  children: [221];
  identifier: %kw_parameter_display_name;
  seed_parents: [219];
  canMatchZeroChars: false
}
221 {
  kind: CharsetLiteral;
  literal: "d", "i", "s", "p", "l", "a", "y", "_", "n", "a", "m", "e";
  seed_parents: [220];
  canMatchZeroChars: false
}
222 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
223 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
224 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
225 {
  kind: Identifier;
  children: [112];
  identifier: %string_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
226 {
  kind: Identifier;
  children: [227];
  identifier: %parameter_unpack;
  seed_parents: [202];
  canMatchZeroChars: false
}
227 {
  kind: Sequence;
  children: [228, 230, 231, 232, 233];
  seed_parents: [226];
  canMatchZeroChars: false
}
228 {
  kind: Identifier;
  children: [229];
  identifier: %kw_parameter_unpack;
  seed_parents: [227];
  canMatchZeroChars: false
}
229 {
  kind: CharsetLiteral;
  literal: "u", "n", "p", "a", "c", "k";
  seed_parents: [228];
  canMatchZeroChars: false
}
230 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
231 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
232 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
233 {
  kind: Identifier;
  children: [234];
  identifier: %boolean_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
234 {
  kind: Choice;
  children: [235, 236];
  seed_parents: [233, 244, 252];
  canMatchZeroChars: false
}
235 {
  kind: CharsetLiteral;
  literal: "t", "r", "u", "e";
  seed_parents: [234];
  canMatchZeroChars: false
}
236 {
  kind: CharsetLiteral;
  literal: "f", "a", "l", "s", "e";
  seed_parents: [234];
  canMatchZeroChars: false
}
237 {
  kind: Identifier;
  children: [238];
  identifier: %parameter_hide;
  seed_parents: [202];
  canMatchZeroChars: false
}
238 {
  kind: Sequence;
  children: [239, 241, 242, 243, 244];
  seed_parents: [237];
  canMatchZeroChars: false
}
239 {
  kind: Identifier;
  children: [240];
  identifier: %kw_parameter_hide;
  seed_parents: [238];
  canMatchZeroChars: false
}
240 {
  kind: CharsetLiteral;
  literal: "h", "i", "d", "e";
  seed_parents: [239];
  canMatchZeroChars: false
}
241 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
242 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
243 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
244 {
  kind: Identifier;
  children: [234];
  identifier: %boolean_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
245 {
  kind: Identifier;
  children: [246];
  identifier: %parameter_merge;
  seed_parents: [202];
  canMatchZeroChars: false
}
246 {
  kind: Sequence;
  children: [247, 249, 250, 251, 252];
  seed_parents: [245];
  canMatchZeroChars: false
}
247 {
  kind: Identifier;
  children: [248];
  identifier: %kw_parameter_merge;
  seed_parents: [246];
  canMatchZeroChars: false
}
248 {
  kind: CharsetLiteral;
  literal: "m", "e", "r", "g", "e";
  seed_parents: [247];
  canMatchZeroChars: false
}
249 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
250 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
251 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
252 {
  kind: Identifier;
  children: [234];
  identifier: %boolean_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
253 {
  kind: Choice;
  children: [254, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
254 {
  kind: OneOrMore;
  children: [255];
  seed_parents: [253];
  canMatchZeroChars: false
}
255 {
  kind: Sequence;
  children: [256, 257, 258, 259];
  seed_parents: [254];
  canMatchZeroChars: false
}
256 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [255];
  canMatchZeroChars: true
}
257 {
  kind: Hidden;
  children: [90];
  seed_parents: [255];
  canMatchZeroChars: false
}
258 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
259 {
  kind: Identifier;
  children: [202];
  identifier: %parameter;
  seed_parents: [];
  canMatchZeroChars: false
}
260 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
261 {
  kind: Hidden;
  children: [262];
  seed_parents: [];
  canMatchZeroChars: false
}
262 {
  kind: CharsetLiteral;
  literal: 10#62;
  seed_parents: [261];
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
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
265 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
266 {
  kind: Identifier;
  children: [54];
  identifier: %definition;
  seed_parents: [];
  canMatchZeroChars: false
}
267 {
  kind: Hidden;
  children: [192];
  seed_parents: [];
  canMatchZeroChars: false
}
268 {
  kind: Identifier;
  children: [269];
  identifier: %grammar_property;
  seed_parents: [32];
  canMatchZeroChars: false
}
269 {
  kind: Sequence;
  children: [270, 272, 273, 274, 275, 276, 277];
  seed_parents: [268];
  canMatchZeroChars: false
}
270 {
  kind: Identifier;
  children: [271];
  identifier: %grammar_property_start;
  seed_parents: [269];
  canMatchZeroChars: false
}
271 {
  kind: CharsetLiteral;
  literal: "@", "s", "t", "a", "r", "t";
  seed_parents: [270];
  canMatchZeroChars: false
}
272 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
273 {
  kind: Hidden;
  children: [51];
  seed_parents: [];
  canMatchZeroChars: false
}
274 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
275 {
  kind: Identifier;
  children: [37];
  identifier: %identifier;
  seed_parents: [];
  canMatchZeroChars: false
}
276 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [];
  canMatchZeroChars: true
}
277 {
  kind: Hidden;
  children: [192];
  seed_parents: [];
  canMatchZeroChars: false
}
278 {
  kind: Choice;
  children: [279, 284];
  seed_parents: [];
  canMatchZeroChars: true
}
279 {
  kind: OneOrMore;
  children: [280];
  seed_parents: [278];
  canMatchZeroChars: false
}
280 {
  kind: Sequence;
  children: [281, 282];
  seed_parents: [279];
  canMatchZeroChars: false
}
281 {
  kind: Identifier;
  children: [3];
  identifier: %blank;
  seed_parents: [280];
  canMatchZeroChars: true
}
282 {
  kind: Identifier;
  children: [32];
  identifier: %statement;
  seed_parents: [280];
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
  kind: Epsilon;
  children: [];
  seed_parents: [3, 46, 62, 77, 92, 106, 115, 120, 125, 154, 164, 174, 176, 181, 215, 253, 278];
  canMatchZeroChars: true
}

