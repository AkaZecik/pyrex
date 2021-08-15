* Parse regex with operators *, |, ?, concat, \xNN, literals and numbered groups ()
* Add handling of +
* Add handling of {n}
* Add handling of {n,}, {,n}, {n,m}
* Handle regexes like a() (it will incorrectly assume it was (a))
* Improve error messages
* Add handling for edge cases
* Add handling for \x00
* Improve interface of parser/tokenizer i.e. determine what should happen if you call them twice or sth like that
* Split parser.cpp into ast.cpp and parser.cpp
* Wrap std::vector<Node *> results into a struct
* Wrap std::vector<InternalNode *> stack into a struct
* Create drop_operators_until_end (operators/groups);
* Count nodes constructed/deconstructed for debugging
* Verify correct usage of "char" type (can it have more than 1 byte?)
* to_string works only under the assumption that AST was constructed with a parser+tokenizer
* Ensure exception safety
* Handle memory allocation fails
* Use proper private/public visibilities

Literal escapes:
* \n
* \r
* \t
* \f
* \\
* \(
* \)
* \*
* \|
* \?
  LPAREN,
  RPAREN,
  STAR,
  UNION,
  QMARK,
  CHAR,
  END,
  };