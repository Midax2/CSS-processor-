# CSS-processor-
The first project of the Algorithms and Data Structures course is about loading CSS blocks and correctly executing commands regarding the loaded data.

Instruction for the project:

CSS Processor
The aim of the task is to write a simple CSS processing engine. The task involves reading CSS sections interleaved with command sections from standard input. CSS sections should be parsed and placed in appropriate structures, while command sections should be parsed and executed, with the results (if any) printed to standard output (after "==").

CSS

Processing begins by reading CSS declarations. CSS is syntactically correct and consists of attribute blocks possibly preceded by selectors. The absence of selectors is allowed (indicating attributes applied to everything).
Selectors are separated by commas. Selectors valid for CSS are allowed, but it can be assumed that they do not contain commas or curly braces.
Attribute blocks are enclosed in curly braces.
Attributes are separated by semicolons and consist of a name (property) and a value separated by a colon. A semicolon may or may not follow the last attribute in a block.
Attribute values can contain constructions legal for CSS, but for simplicity, it can be assumed that strings are not malicious, meaning they do not contain escaped quotation marks, curly braces, or semicolons.
If a specific attribute (name) appears more than once in a block, it should be treated as a single occurrence, with the last value being significant.
Both selectors and attribute names and values do not require semantic interpretation. They are treated as values after discarding leading and trailing white spaces. For example, 'margin-left: 8px', 'margin: 4px 7px 4px 7px' are treated as separate, unrelated attributes with names 'margin-left' and 'margin' and values '8px' and '4px 7px 4px 7px', respectively. Similarly, selectors are treated as values and do not require interpretation. For example, 'h1' and 'h1.theme' are treated as separate, unrelated selectors.
Simplification: CSS does not include comments or @-type selectors, and blocks cannot be nested.
For testing purposes, it can be assumed that no selector or attribute spans multiple lines (multiple separators/and/or attributes can appear on a single line).
Commands

In the following commands, i and j are positive integers (fitting in an int), while n is a legal attribute name. ???? – start of the command section;
**** - resume reading CSS;
? – print the number of CSS blocks;
i,S,? – print the number of selectors for block number i (numbering starts from 1), skip if there is no such block;
i,A,? - print the number of attributes for block number i, skip if there is no such block;
i,S,j – print the j-th selector for the i-th block (block and attribute numbers start from 1), skip if there is no such block or selector;
i,A,n – print the value of the attribute with name n for the i-th block, skip if there is no such attribute;
n,A,? – print the total (across all blocks) number of occurrences of the attribute with name n. (Duplicates should be removed during input parsing). It can be 0;
z,S,? – print the total (across all blocks) number of occurrences of the selector z. It can be 0;
z,E,n – print the value of the attribute with name n for the selector z. If there are multiple occurrences of selector z, take the last one. If it is missing, skip;
i,D,* - delete the entire block number i (i.e., separators+attributes), after successful execution, print "deleted";
i,D,n – delete the attribute with name n from the i-th block. If the operation results in an empty block, it should also be deleted (along with any selectors), after successful execution, print "deleted".

Implementation Notes

Selectors and attributes should be stored as lists. Individual CSS blocks should be organized as a doubly linked list (to efficiently handle the E command). To better utilize memory, the list should encompass an array T=8 of structures representing a block (where T is a compile-time changeable constant) and a counter for the currently occupied structures (to account for deleted elements). Counters should be utilized to speed up operations parameterized by the cell number, i.e., i.

When allocating a new node, an array of T elements is created. When adding elements, if there is free space in the list node, it should be used before allocating new nodes. If an empty array remains after removing elements, the node should be deleted. There is no need to move elements between nodes or merge nodes, etc.
