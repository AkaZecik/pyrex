# PyRex

---

## Python

This project is written in Python 3.9


## Operator precedence

| Precedence | Operator            | Description           | Associativity |
|------------|---------------------|-----------------------|---------------|
|            | ( )                 |                       |               |
|            | ~                   | case-insensitive      | Right-to-left |
|            | *                   | star                  | Right-to-left |
|            | +                   | plus                  | Moze left-to-right? |
|            | {n} {n,} {,m} {n,m} | repeat exact or range |               |
|            | !                   | complement            |               |
|            | -                   | subtraction           |               |
|            | ·                   | concatenation         | Left-to-right |
|            | &                   | intersection          |               |
|            | :                   | xor                   |               |
|            | |                   | union                 |               |
|            |                     |                       |               |


### Precedence

```
0: ()
1: ~
2: *, +, ?, {n}, {n,}, {,m}, {n,m}
3: !
4: -
5: "concat"
6: &
7: :
8: |
```

### Associativity

All binary operators are left-associative.