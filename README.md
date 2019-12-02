# Ling

![Ling Demo](demo.gif)

## Grammar

```
<statement>   ::= <equivalence> | ''

<equivalence> ::= <implication> | <implication> '=' <equivalence>

<implication> ::= <disjunction> | <disjunction> '->' <implication>

<disjunction> ::= <conjuction> | <conjunction> '+' <disjunction>

<conjunction> ::= <negation> | <negation> '*' <conjunction>

<negation>    ::= <identifier> | <negation> '\''

<identifier>  ::= <letter> | <letter> <identifier> | '(' <statement> ')'

<letter>      ::= 'a' ... 'z' | 'A' ... 'Z'
```

## How it works
Ling parses an expression belonging to the above grammar using a recursive descent parser. Then it generates a truth table for the expression and uses the [Quine Mcluskey Algorithm](https://en.wikipedia.org/wiki/Quine-McCluskey_algorithm) to find its prime implicants. It uses these to generate a new, simplified, boolean expression.
