* Parse regex with operators *, |, ?, concat, \xNN, literals and numbered groups ()
* Add handling of +
* Add handling of {n}
* Add handling of {n,}, {,n}, {n,m}
* Improve error messages
* Add handling for edge cases
* Add handling for \x00
* Improve interface of parser/tokenizer i.e. determine what should happen if you call them twice or sth like that

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