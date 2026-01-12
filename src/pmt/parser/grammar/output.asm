0 {
  kind: Identifier;
  children: [1];
  identifier: grammar;
  seed_parents: [];
  canMatchZeroChars: false
}
1 {
  kind: Sequence;
  children: [2, 31, 295, 300];
  seed_parents: [0];
  canMatchZeroChars: false
}
2 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [1];
  canMatchZeroChars: true
}
3 {
  kind: Choice;
  children: [4, 301];
  seed_parents: [2, 46, 49, 90, 103, 106, 112, 115, 117, 124, 146, 150, 152, 156, 161, 163, 176, 188, 196, 201, 204, 206, 207, 212, 217, 224, 226, 239, 241, 247, 249, 258, 260, 266, 268, 273, 275, 277, 280, 282, 289, 291, 293, 298, 300];
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
  identifier: comment;
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
  identifier: single_line_comment;
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
  identifier: single_line_comment_rhs;
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
  identifier: newline;
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
  identifier: any;
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
  identifier: single_line_comment_rhs;
  seed_parents: [];
  canMatchZeroChars: false
}
19 {
  kind: Identifier;
  children: [20];
  identifier: multi_line_comment;
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
  identifier: multi_line_comment_rhs;
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
  identifier: any;
  seed_parents: [25];
  canMatchZeroChars: false
}
27 {
  kind: Identifier;
  children: [23];
  identifier: multi_line_comment_rhs;
  seed_parents: [];
  canMatchZeroChars: false
}
28 {
  kind: Identifier;
  children: [29];
  identifier: whitespace;
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
  identifier: newline;
  seed_parents: [5];
  canMatchZeroChars: false
}
31 {
  kind: Identifier;
  children: [32];
  identifier: statement;
  seed_parents: [1];
  canMatchZeroChars: false
}
32 {
  kind: Choice;
  children: [33, 285];
  seed_parents: [31, 299];
  canMatchZeroChars: false
}
33 {
  kind: Identifier;
  children: [34];
  identifier: production;
  seed_parents: [32];
  canMatchZeroChars: false
}
34 {
  kind: Choice;
  children: [35, 210];
  seed_parents: [33];
  canMatchZeroChars: false
}
35 {
  kind: Sequence;
  children: [36, 46, 47, 49, 50, 207, 208];
  seed_parents: [34];
  canMatchZeroChars: false
}
36 {
  kind: Identifier;
  children: [37];
  identifier: identifier;
  seed_parents: [35];
  canMatchZeroChars: false
}
37 {
  kind: Sequence;
  children: [38, 40];
  seed_parents: [36, 60, 132, 182, 211, 292];
  canMatchZeroChars: false
}
38 {
  kind: Hidden;
  children: [39];
  seed_parents: [37];
  canMatchZeroChars: false
}
39 {
  kind: CharsetLiteral;
  literal: "$";
  seed_parents: [38];
  canMatchZeroChars: false
}
40 {
  kind: Identifier;
  children: [41];
  identifier: identifier_rhs;
  seed_parents: [];
  canMatchZeroChars: false
}
41 {
  kind: Sequence;
  children: [42, 43];
  seed_parents: [40];
  canMatchZeroChars: false
}
42 {
  kind: CharsetLiteral;
  literal: ["A".."Z""_""a".."z"];
  seed_parents: [41, 231];
  canMatchZeroChars: false
}
43 {
  kind: Choice;
  children: [44, 301];
  seed_parents: [];
  canMatchZeroChars: true
}
44 {
  kind: OneOrMore;
  children: [45];
  seed_parents: [43];
  canMatchZeroChars: false
}
45 {
  kind: CharsetLiteral;
  literal: ["0".."9""A".."Z""_""a".."z"];
  seed_parents: [44, 233];
  canMatchZeroChars: false
}
46 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
47 {
  kind: Hidden;
  children: [48];
  seed_parents: [];
  canMatchZeroChars: false
}
48 {
  kind: CharsetLiteral;
  literal: "=";
  seed_parents: [47, 225, 240, 248, 259, 267, 281, 290];
  canMatchZeroChars: false
}
49 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
50 {
  kind: Identifier;
  children: [51];
  identifier: definition;
  seed_parents: [];
  canMatchZeroChars: false
}
51 {
  kind: Identifier;
  children: [52];
  identifier: choices;
  seed_parents: [50, 283];
  canMatchZeroChars: false
}
52 {
  kind: Sequence;
  children: [53, 198, 206];
  seed_parents: [51, 125, 177, 189];
  canMatchZeroChars: false
}
53 {
  kind: Identifier;
  children: [54];
  identifier: sequence;
  seed_parents: [52];
  canMatchZeroChars: false
}
54 {
  kind: Sequence;
  children: [55, 193];
  seed_parents: [53, 205];
  canMatchZeroChars: false
}
55 {
  kind: Identifier;
  children: [56];
  identifier: expression;
  seed_parents: [54];
  canMatchZeroChars: false
}
56 {
  kind: Choice;
  children: [57, 129, 180];
  seed_parents: [55, 197];
  canMatchZeroChars: false
}
57 {
  kind: Identifier;
  children: [58];
  identifier: hidden;
  seed_parents: [56];
  canMatchZeroChars: false
}
58 {
  kind: Choice;
  children: [59, 63, 74, 85, 121];
  seed_parents: [57];
  canMatchZeroChars: false
}
59 {
  kind: Sequence;
  children: [60, 61];
  seed_parents: [58];
  canMatchZeroChars: false
}
60 {
  kind: Identifier;
  children: [37];
  identifier: identifier;
  seed_parents: [59];
  canMatchZeroChars: false
}
61 {
  kind: Identifier;
  children: [62];
  identifier: tilde;
  seed_parents: [];
  canMatchZeroChars: false
}
62 {
  kind: CharsetLiteral;
  literal: "~";
  seed_parents: [61, 73, 84, 120, 128];
  canMatchZeroChars: false
}
63 {
  kind: Sequence;
  children: [64, 73];
  seed_parents: [58];
  canMatchZeroChars: false
}
64 {
  kind: Identifier;
  children: [65];
  identifier: string_literal;
  seed_parents: [63];
  canMatchZeroChars: false
}
65 {
  kind: Sequence;
  children: [66, 68, 72];
  seed_parents: [64, 166, 183, 242];
  canMatchZeroChars: false
}
66 {
  kind: Hidden;
  children: [67];
  seed_parents: [65];
  canMatchZeroChars: false
}
67 {
  kind: CharsetLiteral;
  literal: 10#34;
  seed_parents: [66, 72, 99, 101, 229, 234];
  canMatchZeroChars: false
}
68 {
  kind: Choice;
  children: [69, 301];
  seed_parents: [];
  canMatchZeroChars: true
}
69 {
  kind: OneOrMore;
  children: [70];
  seed_parents: [68];
  canMatchZeroChars: false
}
70 {
  kind: Identifier;
  children: [71];
  identifier: string_middle;
  seed_parents: [69];
  canMatchZeroChars: false
}
71 {
  kind: CharsetLiteral;
  literal: [" ".."!""#".."~"];
  seed_parents: [70, 100];
  canMatchZeroChars: false
}
72 {
  kind: Hidden;
  children: [67];
  seed_parents: [];
  canMatchZeroChars: false
}
73 {
  kind: Identifier;
  children: [62];
  identifier: tilde;
  seed_parents: [];
  canMatchZeroChars: false
}
74 {
  kind: Sequence;
  children: [75, 84];
  seed_parents: [58];
  canMatchZeroChars: false
}
75 {
  kind: Identifier;
  children: [76];
  identifier: integer_literal;
  seed_parents: [74];
  canMatchZeroChars: false
}
76 {
  kind: Sequence;
  children: [77, 80, 81];
  seed_parents: [75, 102, 149, 155, 162, 169, 184];
  canMatchZeroChars: false
}
77 {
  kind: Identifier;
  children: [78];
  identifier: integer_literal_lhs;
  seed_parents: [76];
  canMatchZeroChars: false
}
78 {
  kind: OneOrMore;
  children: [79];
  seed_parents: [77];
  canMatchZeroChars: false
}
79 {
  kind: CharsetLiteral;
  literal: ["0".."9"];
  seed_parents: [78];
  canMatchZeroChars: false
}
80 {
  kind: CharsetLiteral;
  literal: "#";
  seed_parents: [];
  canMatchZeroChars: false
}
81 {
  kind: Identifier;
  children: [82];
  identifier: integer_literal_rhs;
  seed_parents: [];
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
  literal: ["0".."9""A".."Z""a".."z"];
  seed_parents: [82];
  canMatchZeroChars: false
}
84 {
  kind: Identifier;
  children: [62];
  identifier: tilde;
  seed_parents: [];
  canMatchZeroChars: false
}
85 {
  kind: Sequence;
  children: [86, 120];
  seed_parents: [58];
  canMatchZeroChars: false
}
86 {
  kind: Identifier;
  children: [87];
  identifier: charset;
  seed_parents: [85];
  canMatchZeroChars: false
}
87 {
  kind: Sequence;
  children: [88, 90, 91, 109, 117, 118];
  seed_parents: [86, 172, 185];
  canMatchZeroChars: false
}
88 {
  kind: Hidden;
  children: [89];
  seed_parents: [87];
  canMatchZeroChars: false
}
89 {
  kind: CharsetLiteral;
  literal: "[";
  seed_parents: [88];
  canMatchZeroChars: false
}
90 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
91 {
  kind: Identifier;
  children: [92];
  identifier: charset_item;
  seed_parents: [];
  canMatchZeroChars: false
}
92 {
  kind: Choice;
  children: [93, 108];
  seed_parents: [91, 116];
  canMatchZeroChars: false
}
93 {
  kind: Identifier;
  children: [94];
  identifier: charset_range;
  seed_parents: [92];
  canMatchZeroChars: false
}
94 {
  kind: Sequence;
  children: [95, 103, 104, 106, 107];
  seed_parents: [93];
  canMatchZeroChars: false
}
95 {
  kind: Identifier;
  children: [96];
  identifier: charset_literal;
  seed_parents: [94];
  canMatchZeroChars: false
}
96 {
  kind: Choice;
  children: [97, 102];
  seed_parents: [95, 107, 108];
  canMatchZeroChars: false
}
97 {
  kind: Identifier;
  children: [98];
  identifier: character_literal;
  seed_parents: [96];
  canMatchZeroChars: false
}
98 {
  kind: Sequence;
  children: [99, 100, 101];
  seed_parents: [97];
  canMatchZeroChars: false
}
99 {
  kind: Hidden;
  children: [67];
  seed_parents: [98];
  canMatchZeroChars: false
}
100 {
  kind: Identifier;
  children: [71];
  identifier: string_middle;
  seed_parents: [];
  canMatchZeroChars: false
}
101 {
  kind: Hidden;
  children: [67];
  seed_parents: [];
  canMatchZeroChars: false
}
102 {
  kind: Identifier;
  children: [76];
  identifier: integer_literal;
  seed_parents: [96];
  canMatchZeroChars: false
}
103 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
104 {
  kind: Hidden;
  children: [105];
  seed_parents: [];
  canMatchZeroChars: false
}
105 {
  kind: CharsetLiteral;
  literal: ".", ".";
  seed_parents: [104];
  canMatchZeroChars: false
}
106 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
107 {
  kind: Identifier;
  children: [96];
  identifier: charset_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
108 {
  kind: Identifier;
  children: [96];
  identifier: charset_literal;
  seed_parents: [92];
  canMatchZeroChars: false
}
109 {
  kind: Choice;
  children: [110, 301];
  seed_parents: [];
  canMatchZeroChars: true
}
110 {
  kind: OneOrMore;
  children: [111];
  seed_parents: [109];
  canMatchZeroChars: false
}
111 {
  kind: Sequence;
  children: [112, 113, 115, 116];
  seed_parents: [110];
  canMatchZeroChars: false
}
112 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [111];
  canMatchZeroChars: true
}
113 {
  kind: Hidden;
  children: [114];
  seed_parents: [111];
  canMatchZeroChars: false
}
114 {
  kind: CharsetLiteral;
  literal: ",";
  seed_parents: [113, 151, 274];
  canMatchZeroChars: false
}
115 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
116 {
  kind: Identifier;
  children: [92];
  identifier: charset_item;
  seed_parents: [];
  canMatchZeroChars: false
}
117 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
118 {
  kind: Hidden;
  children: [119];
  seed_parents: [];
  canMatchZeroChars: false
}
119 {
  kind: CharsetLiteral;
  literal: "]";
  seed_parents: [118];
  canMatchZeroChars: false
}
120 {
  kind: Identifier;
  children: [62];
  identifier: tilde;
  seed_parents: [];
  canMatchZeroChars: false
}
121 {
  kind: Sequence;
  children: [122, 124, 125, 126, 128];
  seed_parents: [58];
  canMatchZeroChars: false
}
122 {
  kind: Hidden;
  children: [123];
  seed_parents: [121];
  canMatchZeroChars: false
}
123 {
  kind: CharsetLiteral;
  literal: "(";
  seed_parents: [122, 175, 187];
  canMatchZeroChars: false
}
124 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
125 {
  kind: Identifier;
  children: [52];
  identifier: choices;
  seed_parents: [];
  canMatchZeroChars: false
}
126 {
  kind: Hidden;
  children: [127];
  seed_parents: [];
  canMatchZeroChars: false
}
127 {
  kind: CharsetLiteral;
  literal: ")";
  seed_parents: [126, 178, 190];
  canMatchZeroChars: false
}
128 {
  kind: Identifier;
  children: [62];
  identifier: tilde;
  seed_parents: [];
  canMatchZeroChars: false
}
129 {
  kind: Identifier;
  children: [130];
  identifier: repetition;
  seed_parents: [56];
  canMatchZeroChars: false
}
130 {
  kind: Choice;
  children: [131, 165, 168, 171, 174];
  seed_parents: [129];
  canMatchZeroChars: false
}
131 {
  kind: Sequence;
  children: [132, 133];
  seed_parents: [130];
  canMatchZeroChars: false
}
132 {
  kind: Identifier;
  children: [37];
  identifier: identifier;
  seed_parents: [131];
  canMatchZeroChars: false
}
133 {
  kind: Identifier;
  children: [134];
  identifier: repetition_postfix;
  seed_parents: [];
  canMatchZeroChars: false
}
134 {
  kind: Choice;
  children: [135, 137, 139, 141];
  seed_parents: [133, 167, 170, 173, 179];
  canMatchZeroChars: false
}
135 {
  kind: Identifier;
  children: [136];
  identifier: question;
  seed_parents: [134];
  canMatchZeroChars: false
}
136 {
  kind: CharsetLiteral;
  literal: "?";
  seed_parents: [135];
  canMatchZeroChars: false
}
137 {
  kind: Identifier;
  children: [138];
  identifier: star;
  seed_parents: [134];
  canMatchZeroChars: false
}
138 {
  kind: CharsetLiteral;
  literal: "*";
  seed_parents: [137];
  canMatchZeroChars: false
}
139 {
  kind: Identifier;
  children: [140];
  identifier: plus;
  seed_parents: [134];
  canMatchZeroChars: false
}
140 {
  kind: CharsetLiteral;
  literal: "+";
  seed_parents: [139];
  canMatchZeroChars: false
}
141 {
  kind: Identifier;
  children: [142];
  identifier: repetition_range;
  seed_parents: [134];
  canMatchZeroChars: false
}
142 {
  kind: Choice;
  children: [143, 159];
  seed_parents: [141];
  canMatchZeroChars: false
}
143 {
  kind: Sequence;
  children: [144, 146, 147, 151, 152, 153, 157];
  seed_parents: [142];
  canMatchZeroChars: false
}
144 {
  kind: Hidden;
  children: [145];
  seed_parents: [143];
  canMatchZeroChars: false
}
145 {
  kind: CharsetLiteral;
  literal: "{";
  seed_parents: [144, 160];
  canMatchZeroChars: false
}
146 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
147 {
  kind: Choice;
  children: [148, 301];
  seed_parents: [];
  canMatchZeroChars: true
}
148 {
  kind: Sequence;
  children: [149, 150];
  seed_parents: [147];
  canMatchZeroChars: false
}
149 {
  kind: Identifier;
  children: [76];
  identifier: integer_literal;
  seed_parents: [148];
  canMatchZeroChars: false
}
150 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
151 {
  kind: Identifier;
  children: [114];
  identifier: comma;
  seed_parents: [];
  canMatchZeroChars: false
}
152 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
153 {
  kind: Choice;
  children: [154, 301];
  seed_parents: [];
  canMatchZeroChars: true
}
154 {
  kind: Sequence;
  children: [155, 156];
  seed_parents: [153];
  canMatchZeroChars: false
}
155 {
  kind: Identifier;
  children: [76];
  identifier: integer_literal;
  seed_parents: [154];
  canMatchZeroChars: false
}
156 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
157 {
  kind: Hidden;
  children: [158];
  seed_parents: [];
  canMatchZeroChars: false
}
158 {
  kind: CharsetLiteral;
  literal: "}";
  seed_parents: [157, 164];
  canMatchZeroChars: false
}
159 {
  kind: Sequence;
  children: [160, 161, 162, 163, 164];
  seed_parents: [142];
  canMatchZeroChars: false
}
160 {
  kind: Hidden;
  children: [145];
  seed_parents: [159];
  canMatchZeroChars: false
}
161 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
162 {
  kind: Identifier;
  children: [76];
  identifier: integer_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
163 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
164 {
  kind: Hidden;
  children: [158];
  seed_parents: [];
  canMatchZeroChars: false
}
165 {
  kind: Sequence;
  children: [166, 167];
  seed_parents: [130];
  canMatchZeroChars: false
}
166 {
  kind: Identifier;
  children: [65];
  identifier: string_literal;
  seed_parents: [165];
  canMatchZeroChars: false
}
167 {
  kind: Identifier;
  children: [134];
  identifier: repetition_postfix;
  seed_parents: [];
  canMatchZeroChars: false
}
168 {
  kind: Sequence;
  children: [169, 170];
  seed_parents: [130];
  canMatchZeroChars: false
}
169 {
  kind: Identifier;
  children: [76];
  identifier: integer_literal;
  seed_parents: [168];
  canMatchZeroChars: false
}
170 {
  kind: Identifier;
  children: [134];
  identifier: repetition_postfix;
  seed_parents: [];
  canMatchZeroChars: false
}
171 {
  kind: Sequence;
  children: [172, 173];
  seed_parents: [130];
  canMatchZeroChars: false
}
172 {
  kind: Identifier;
  children: [87];
  identifier: charset;
  seed_parents: [171];
  canMatchZeroChars: false
}
173 {
  kind: Identifier;
  children: [134];
  identifier: repetition_postfix;
  seed_parents: [];
  canMatchZeroChars: false
}
174 {
  kind: Sequence;
  children: [175, 176, 177, 178, 179];
  seed_parents: [130];
  canMatchZeroChars: false
}
175 {
  kind: Hidden;
  children: [123];
  seed_parents: [174];
  canMatchZeroChars: false
}
176 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
177 {
  kind: Identifier;
  children: [52];
  identifier: choices;
  seed_parents: [];
  canMatchZeroChars: false
}
178 {
  kind: Hidden;
  children: [127];
  seed_parents: [];
  canMatchZeroChars: false
}
179 {
  kind: Identifier;
  children: [134];
  identifier: repetition_postfix;
  seed_parents: [];
  canMatchZeroChars: false
}
180 {
  kind: Identifier;
  children: [181];
  identifier: plain;
  seed_parents: [56];
  canMatchZeroChars: false
}
181 {
  kind: Choice;
  children: [182, 183, 184, 185, 186, 191];
  seed_parents: [180];
  canMatchZeroChars: false
}
182 {
  kind: Identifier;
  children: [37];
  identifier: identifier;
  seed_parents: [181];
  canMatchZeroChars: false
}
183 {
  kind: Identifier;
  children: [65];
  identifier: string_literal;
  seed_parents: [181];
  canMatchZeroChars: false
}
184 {
  kind: Identifier;
  children: [76];
  identifier: integer_literal;
  seed_parents: [181];
  canMatchZeroChars: false
}
185 {
  kind: Identifier;
  children: [87];
  identifier: charset;
  seed_parents: [181];
  canMatchZeroChars: false
}
186 {
  kind: Sequence;
  children: [187, 188, 189, 190];
  seed_parents: [181];
  canMatchZeroChars: false
}
187 {
  kind: Hidden;
  children: [123];
  seed_parents: [186];
  canMatchZeroChars: false
}
188 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
189 {
  kind: Identifier;
  children: [52];
  identifier: choices;
  seed_parents: [];
  canMatchZeroChars: false
}
190 {
  kind: Hidden;
  children: [127];
  seed_parents: [];
  canMatchZeroChars: false
}
191 {
  kind: Identifier;
  children: [192];
  identifier: kw_epsilon;
  seed_parents: [181];
  canMatchZeroChars: false
}
192 {
  kind: CharsetLiteral;
  literal: "e", "p", "s", "i", "l", "o", "n";
  seed_parents: [191];
  canMatchZeroChars: false
}
193 {
  kind: Choice;
  children: [194, 301];
  seed_parents: [];
  canMatchZeroChars: true
}
194 {
  kind: OneOrMore;
  children: [195];
  seed_parents: [193];
  canMatchZeroChars: false
}
195 {
  kind: Sequence;
  children: [196, 197];
  seed_parents: [194];
  canMatchZeroChars: false
}
196 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [195];
  canMatchZeroChars: true
}
197 {
  kind: Identifier;
  children: [56];
  identifier: expression;
  seed_parents: [195];
  canMatchZeroChars: false
}
198 {
  kind: Choice;
  children: [199, 301];
  seed_parents: [];
  canMatchZeroChars: true
}
199 {
  kind: OneOrMore;
  children: [200];
  seed_parents: [198];
  canMatchZeroChars: false
}
200 {
  kind: Sequence;
  children: [201, 202, 204, 205];
  seed_parents: [199];
  canMatchZeroChars: false
}
201 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [200];
  canMatchZeroChars: true
}
202 {
  kind: Hidden;
  children: [203];
  seed_parents: [200];
  canMatchZeroChars: false
}
203 {
  kind: CharsetLiteral;
  literal: "|";
  seed_parents: [202];
  canMatchZeroChars: false
}
204 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
205 {
  kind: Identifier;
  children: [54];
  identifier: sequence;
  seed_parents: [];
  canMatchZeroChars: false
}
206 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
207 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
208 {
  kind: Hidden;
  children: [209];
  seed_parents: [];
  canMatchZeroChars: false
}
209 {
  kind: CharsetLiteral;
  literal: ";";
  seed_parents: [208, 284, 294];
  canMatchZeroChars: false
}
210 {
  kind: Sequence;
  children: [211, 212, 213, 215, 278, 280, 281, 282, 283, 284];
  seed_parents: [34];
  canMatchZeroChars: false
}
211 {
  kind: Identifier;
  children: [37];
  identifier: identifier;
  seed_parents: [210];
  canMatchZeroChars: false
}
212 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
213 {
  kind: Hidden;
  children: [214];
  seed_parents: [];
  canMatchZeroChars: false
}
214 {
  kind: CharsetLiteral;
  literal: 10#60;
  seed_parents: [213];
  canMatchZeroChars: false
}
215 {
  kind: Identifier;
  children: [216];
  identifier: parameter_list;
  seed_parents: [];
  canMatchZeroChars: false
}
216 {
  kind: Sequence;
  children: [217, 218, 270, 277];
  seed_parents: [215];
  canMatchZeroChars: false
}
217 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [216];
  canMatchZeroChars: true
}
218 {
  kind: Identifier;
  children: [219];
  identifier: parameter;
  seed_parents: [216];
  canMatchZeroChars: false
}
219 {
  kind: Choice;
  children: [220, 235, 243, 254, 262];
  seed_parents: [218, 276];
  canMatchZeroChars: false
}
220 {
  kind: Identifier;
  children: [221];
  identifier: parameter_id;
  seed_parents: [219];
  canMatchZeroChars: false
}
221 {
  kind: Sequence;
  children: [222, 224, 225, 226, 227];
  seed_parents: [220];
  canMatchZeroChars: false
}
222 {
  kind: Identifier;
  children: [223];
  identifier: kw_parameter_id;
  seed_parents: [221];
  canMatchZeroChars: false
}
223 {
  kind: CharsetLiteral;
  literal: "i", "d";
  seed_parents: [222];
  canMatchZeroChars: false
}
224 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
225 {
  kind: Hidden;
  children: [48];
  seed_parents: [];
  canMatchZeroChars: false
}
226 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
227 {
  kind: Identifier;
  children: [228];
  identifier: id_string_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
228 {
  kind: Sequence;
  children: [229, 230, 234];
  seed_parents: [227];
  canMatchZeroChars: false
}
229 {
  kind: Hidden;
  children: [67];
  seed_parents: [228];
  canMatchZeroChars: false
}
230 {
  kind: Identifier;
  children: [231];
  identifier: id_string_middle;
  seed_parents: [];
  canMatchZeroChars: false
}
231 {
  kind: Sequence;
  children: [42, 232];
  seed_parents: [230];
  canMatchZeroChars: false
}
232 {
  kind: Choice;
  children: [233, 301];
  seed_parents: [];
  canMatchZeroChars: true
}
233 {
  kind: OneOrMore;
  children: [45];
  seed_parents: [232];
  canMatchZeroChars: false
}
234 {
  kind: Hidden;
  children: [67];
  seed_parents: [];
  canMatchZeroChars: false
}
235 {
  kind: Identifier;
  children: [236];
  identifier: parameter_displayname;
  seed_parents: [219];
  canMatchZeroChars: false
}
236 {
  kind: Sequence;
  children: [237, 239, 240, 241, 242];
  seed_parents: [235];
  canMatchZeroChars: false
}
237 {
  kind: Identifier;
  children: [238];
  identifier: kw_parameter_display_name;
  seed_parents: [236];
  canMatchZeroChars: false
}
238 {
  kind: CharsetLiteral;
  literal: "d", "i", "s", "p", "l", "a", "y", "_", "n", "a", "m", "e";
  seed_parents: [237];
  canMatchZeroChars: false
}
239 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
240 {
  kind: Hidden;
  children: [48];
  seed_parents: [];
  canMatchZeroChars: false
}
241 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
242 {
  kind: Identifier;
  children: [65];
  identifier: string_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
243 {
  kind: Identifier;
  children: [244];
  identifier: parameter_unpack;
  seed_parents: [219];
  canMatchZeroChars: false
}
244 {
  kind: Sequence;
  children: [245, 247, 248, 249, 250];
  seed_parents: [243];
  canMatchZeroChars: false
}
245 {
  kind: Identifier;
  children: [246];
  identifier: kw_parameter_unpack;
  seed_parents: [244];
  canMatchZeroChars: false
}
246 {
  kind: CharsetLiteral;
  literal: "u", "n", "p", "a", "c", "k";
  seed_parents: [245];
  canMatchZeroChars: false
}
247 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
248 {
  kind: Hidden;
  children: [48];
  seed_parents: [];
  canMatchZeroChars: false
}
249 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
250 {
  kind: Identifier;
  children: [251];
  identifier: boolean_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
251 {
  kind: Choice;
  children: [252, 253];
  seed_parents: [250, 261, 269];
  canMatchZeroChars: false
}
252 {
  kind: CharsetLiteral;
  literal: "t", "r", "u", "e";
  seed_parents: [251];
  canMatchZeroChars: false
}
253 {
  kind: CharsetLiteral;
  literal: "f", "a", "l", "s", "e";
  seed_parents: [251];
  canMatchZeroChars: false
}
254 {
  kind: Identifier;
  children: [255];
  identifier: parameter_hide;
  seed_parents: [219];
  canMatchZeroChars: false
}
255 {
  kind: Sequence;
  children: [256, 258, 259, 260, 261];
  seed_parents: [254];
  canMatchZeroChars: false
}
256 {
  kind: Identifier;
  children: [257];
  identifier: kw_parameter_hide;
  seed_parents: [255];
  canMatchZeroChars: false
}
257 {
  kind: CharsetLiteral;
  literal: "h", "i", "d", "e";
  seed_parents: [256];
  canMatchZeroChars: false
}
258 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
259 {
  kind: Hidden;
  children: [48];
  seed_parents: [];
  canMatchZeroChars: false
}
260 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
261 {
  kind: Identifier;
  children: [251];
  identifier: boolean_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
262 {
  kind: Identifier;
  children: [263];
  identifier: parameter_merge;
  seed_parents: [219];
  canMatchZeroChars: false
}
263 {
  kind: Sequence;
  children: [264, 266, 267, 268, 269];
  seed_parents: [262];
  canMatchZeroChars: false
}
264 {
  kind: Identifier;
  children: [265];
  identifier: kw_parameter_merge;
  seed_parents: [263];
  canMatchZeroChars: false
}
265 {
  kind: CharsetLiteral;
  literal: "m", "e", "r", "g", "e";
  seed_parents: [264];
  canMatchZeroChars: false
}
266 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
267 {
  kind: Hidden;
  children: [48];
  seed_parents: [];
  canMatchZeroChars: false
}
268 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
269 {
  kind: Identifier;
  children: [251];
  identifier: boolean_literal;
  seed_parents: [];
  canMatchZeroChars: false
}
270 {
  kind: Choice;
  children: [271, 301];
  seed_parents: [];
  canMatchZeroChars: true
}
271 {
  kind: OneOrMore;
  children: [272];
  seed_parents: [270];
  canMatchZeroChars: false
}
272 {
  kind: Sequence;
  children: [273, 274, 275, 276];
  seed_parents: [271];
  canMatchZeroChars: false
}
273 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [272];
  canMatchZeroChars: true
}
274 {
  kind: Hidden;
  children: [114];
  seed_parents: [272];
  canMatchZeroChars: false
}
275 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
276 {
  kind: Identifier;
  children: [219];
  identifier: parameter;
  seed_parents: [];
  canMatchZeroChars: false
}
277 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
278 {
  kind: Hidden;
  children: [279];
  seed_parents: [];
  canMatchZeroChars: false
}
279 {
  kind: CharsetLiteral;
  literal: 10#62;
  seed_parents: [278];
  canMatchZeroChars: false
}
280 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
281 {
  kind: Hidden;
  children: [48];
  seed_parents: [];
  canMatchZeroChars: false
}
282 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
283 {
  kind: Identifier;
  children: [51];
  identifier: definition;
  seed_parents: [];
  canMatchZeroChars: false
}
284 {
  kind: Hidden;
  children: [209];
  seed_parents: [];
  canMatchZeroChars: false
}
285 {
  kind: Identifier;
  children: [286];
  identifier: grammar_property;
  seed_parents: [32];
  canMatchZeroChars: false
}
286 {
  kind: Sequence;
  children: [287, 289, 290, 291, 292, 293, 294];
  seed_parents: [285];
  canMatchZeroChars: false
}
287 {
  kind: Identifier;
  children: [288];
  identifier: grammar_property_start;
  seed_parents: [286];
  canMatchZeroChars: false
}
288 {
  kind: CharsetLiteral;
  literal: "@", "s", "t", "a", "r", "t";
  seed_parents: [287];
  canMatchZeroChars: false
}
289 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
290 {
  kind: Hidden;
  children: [48];
  seed_parents: [];
  canMatchZeroChars: false
}
291 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
292 {
  kind: Identifier;
  children: [37];
  identifier: identifier;
  seed_parents: [];
  canMatchZeroChars: false
}
293 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
294 {
  kind: Hidden;
  children: [209];
  seed_parents: [];
  canMatchZeroChars: false
}
295 {
  kind: Choice;
  children: [296, 301];
  seed_parents: [];
  canMatchZeroChars: true
}
296 {
  kind: OneOrMore;
  children: [297];
  seed_parents: [295];
  canMatchZeroChars: false
}
297 {
  kind: Sequence;
  children: [298, 299];
  seed_parents: [296];
  canMatchZeroChars: false
}
298 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [297];
  canMatchZeroChars: true
}
299 {
  kind: Identifier;
  children: [32];
  identifier: statement;
  seed_parents: [297];
  canMatchZeroChars: false
}
300 {
  kind: Identifier;
  children: [3];
  identifier: blank;
  seed_parents: [];
  canMatchZeroChars: true
}
301 {
  kind: Epsilon;
  children: [];
  seed_parents: [3, 43, 68, 109, 147, 153, 193, 198, 232, 270, 295];
  canMatchZeroChars: true
}

