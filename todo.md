* operator %
* operator &
* operator -, xor
* znak \e, \#
* znak .
* charset [a-ce-x]
* przyjmowanie tekstu poprzez iterator do kolejnych znakow, zeby moc obslugiwac \0
* reprezentacja przedzialu znakow za pomoca jednego wierzcholka i wielu krawedzi
* reprezentacja przedzialu znakow za pomoca jednego wierzcholka i jednej krawedzi
* byc moze w obecnej implementacji nie jest potrzebne trzymanie znaku na wierzcholku
  i wystarcza znaki na krawedziach
* usunac 'id' z lisci w AST
* usunac 'id' i 'c' z wierzcholkow w NFA
* ustalanie alfabetu przed parsowaniem
* optymalizowanie skladania operatorow, np. R**, R*?, R?*, ((R*)*)*, R*{3}, etc.
* rename "empty edge" to "epsilon edge"
* rozbij nfa.cpp na nfa.h+nfa.cpp oraz rozbij nfa.cpp na czesci, ktore maja wspolne przeznaczenie
  np. traverse w innym pliku niz operatory, Node tez moze w innym pliku
* ograniczyc liczbe stanow, ktore moze miec NFA/DFA do pewnej stalej
* handlowanie errorow tokenizacji i parsowania (osobny plik na errory?)
























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