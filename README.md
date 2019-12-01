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

Ling parses an expression using the above grammar, generates a truth table, then uses the Quine-McCluskey method to find prime implicants. If the expression isn't a tautology or a contradiction, it prints out prime implicants and a prime implicant table. I'm stuck on Petrick's method right now, but eventually I'd like it to be able to generate a new, simplified expression.