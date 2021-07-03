# TeaLang Interpreter CPP20

Presented here is a TeaLang interpreter written in C++ (20). This assignment forms part of the CPS2000 unit which was taken in partial fulfilment of the
requirements for the degree of B.Sc. in Artificial Intelligence.

The interpreter and sub processes were developed and written to follow the specifications in the CPS2000_Assignment_2021 (Part1) pdf.

The TeaLang language follows the following EBNF:


```
<Letter> ::= [A-Za-z]

<Digit> ::= [0-9]

<Printable> ::= [\textbackslash x20-\textbackslash x7E]

<Type> ::= `float' | `int' | `bool' | `string'

<BooleanLiteral> ::= `true' | `false'

<IntegerLiteral> ::= <Digit> \{ <Digit> \}

<FloatLiteral> ::= <Digit> \{ <Digit> \} `.' <Digit> \{ <Digit> \}

<StringLiteral> ::= `"' \{ <Printable> \} `"'

<Literal> ::= <BooleanLiteral> |
              <IntegerLiteral> |
              <FloatLiteral>   |
              <StringLiteral>

<Identifier> ::= ( `\_' | <Letter> )  \{ `\_' | <Letter> | <Digit> \}

<MultiplicativeOp> ::= `*' | `/' | `and' 

<AdditiveOp> ::= `+' | `-' | `or' 

<RelationalOp> ::= `<' | `>' | `==' | `!=' | `<=' | `>=' 

<ActualParams> ::= <Expression> \{ `,' <Expression> \}

<FunctionCall> ::= <Identifier> `(' [ <ActualParams> ] `)' 

<SubExpression> ::= `(' <Expression> `)' 

<Unary> ::= ( `-' | `not' ) <Expression>

<Factor> ::=  <Literal>       |
              <Identifier>    |
              <FunctionCall>  |
              <SubExpression> |
              <Unary>         

<Term> ::= <Factor> \{ <MultiplicativeOp> <Factor> \}

<SimpleExpression> ::= <Term> \{ <AdditiveOp> <Term> \}

<Expression> ::= <SimpleExpression> \{ <RelationalOp> <SimpleExpression> \}

<Assignment> ::= <Identifier> `=' <Expression>

<VariableDecl> ::= `let' <Identifier> `:' <Type> `=' <Expression>

<PrintStatement> ::= `print' <Expression>

<RtrnStatement> ::= `return' <Expression>

<IfStatement> ::= `if' `(' <Expression> `)' <Block> [ `else' <Block> ]

<ForStatement> ::= `for' `(' [ <VariableDecl> ] ';' <Expression> ';' [ <Assignment> ] `)' <Block>

<WhileStatement> ::= `while' `(' <Expression> `)' <Block> 

<FormalParam> ::= <Identifier> `:' <Type>

<FormalParams> ::= <FormalParam> \{ `,' <FormalParam> \}

<FunctionDecl> ::= <type> <Identifier> `(' [ <FormalParams> ] `)' <Block>

<Statement> ::=	<VariableDecl> `;'   |
                <Assignment> `;'     |
                <PrintStatement> `;' |
                <IfStatement>        |
                <ForStatement>       |
                <WhileStatement>     |
                <RtrnStatement> `;'  |
                <FunctionDecl>       |
                <Block>              

<Block> ::= `{' \{ <Statement> \} `}' 

<Program> ::= \{ <Statement> \}

```

The following is a syntactically and semantically correct TeaLang program:

```
float Square (x : float) {
    return x*x;
}
bool XGreaterThanY (x : float , y : float) {
    let ans : bool = true;
    if (y > x) {ans = false ;}
    return ans;
}
bool XGreaterThanYv2 (x : float , y : float) {
    return x > y;
}
float AverageOfThree (x : float , y : float , z : float) {
    let total : float = x + y + z ;
    return total / 3.0;
}

string JoinStr (s1 : string , s2 : string) {
    let s3 : string = s1 + s2 ;
    return s3 ;
}
let x : float = 2.4;
let y : float = Square (2.5);
print y ; //6.25
print XGreaterThanY (x , 2.3 ) ; // t r u e
print XGreaterThanYv2 (Square ( 1.5 ) , y ) ; // f a l s e
print AverageOfThree (x , y , 1.2 ) ; //3.28
print JoinStr(\"Hello\" , \"World\" ) ; // Hello World
for(let ii : int = 0; ii < 10; ii = ii + 1){
   print ii;
}
let ii : int = 0;
while (ii < 10){
   print ii;
   ii = ii + 1;
}
```

A full detailed report can be found in Report.pdf
