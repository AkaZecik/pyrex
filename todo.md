* Parse regex with operators *, |, ?, concat, \xNN and literals
* Add handling of +
* Add handling of {n}
* Add handling of {n,}, {,n}, {n,m}
* Improve error messages
* Add handling for edge cases
* Add handling for \x00

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