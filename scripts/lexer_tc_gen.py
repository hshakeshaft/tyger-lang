"""
const char *prog = "<contents of input file>";

auto lexer_tests = std::vector<Lexer_TC>{
    { <kind>, sv{ &prog[<pos>], <len> }, <location> },
    ...
};
"""
#!/usr/bin/env python3
#
# File to automatically generate updated test cases for the lexer from an input file.
#
import enum
from dataclasses import dataclass

#
# read file
#
FNAME = "prog.tyger"
content = None
with open(FNAME, "r") as f:
    content = f.read()

print(content)
print("=" * 80)

#
# lex input
#
class TokenKind(enum.Enum):
    def __str__(self):
        return f"TK_{self.value.upper()}"

    Eof     = "eof"
    Illeagl = "illegal"

    # operators
    plus = "plus"
    minus = "minus"
    asterisk = "asterisk"
    slash = "slash"
    assign = "assign"
    bang = "bang"
    eq = "eq"
    neq = "neq"
    lt = "lt"
    gt = "gt"
    lte = "lte"
    gte = "gte"
    lor = "lor"
    land = "land"

    # delimiters
    lparen = "lparen"
    rparen = "rparen"
    lbrace = "lbrace"
    rbrace = "rbrace"
    lbracket = "lbracket"
    rbracket = "rbracket"

    semicolon = "semicolon"
    colon = "colon"
    comma = "comma"
    period = "period"

    # literals and idents
    ident = "ident"
    int_lit = "int_lit"
    float_lit = "float_lit"
    stirng_lit = "string_lit"

    # keywords
    var_ = "var"
    if_ = "if"
    else_ = "else"
    func_ = "func"
    return_ = "return"
    true_ = "true"
    false_ = "false"
    nil_ = "nil"

    # builtins
    println = "println"


OPERATOR = {
    "+": TokenKind.plus,
    "-": TokenKind.minus,
    "*": TokenKind.asterisk,
    "/": TokenKind.slash,
    "=": TokenKind.assign,
    "!": TokenKind.bang,
    "<": TokenKind.lt,
    ">": TokenKind.gt,
    "!=": TokenKind.neq,
    "==": TokenKind.eq,
    "<=": TokenKind.lte,
    ">=": TokenKind.gte,
    "||": TokenKind.lor,
    "&&": TokenKind.land,
}

DELIMITERS = {
    "(": TokenKind.lparen,
    ")": TokenKind.rparen,
    "{": TokenKind.lbrace,
    "}": TokenKind.rbrace,
    "[": TokenKind.lbracket,
    "]": TokenKind.rbracket,
    ";": TokenKind.semicolon,
    ":": TokenKind.colon,
    ",": TokenKind.comma,
    ".": TokenKind.period,
}

KEWYORDS_BUILTIN = {
    "var": TokenKind.var_,
    "if": TokenKind.if_,
    "else": TokenKind.else_,
    "func": TokenKind.func_,
    "return": TokenKind.return_,
    "true": TokenKind.true_,
    "false": TokenKind.false_,
    "nil": TokenKind.nil_,
    "println": TokenKind.println,
}

@dataclass
class Token:
    pos: int
    line: int
    col: int
    kind: TokenKind = TokenKind.Eof
    literal_len: int = 1

    def __str__(self):
        """Return token formatted in a style where it would be a literal declaration.

        format is (with designated initialisers): 
        Token{ 
            .kind = <kind>, 
            .location = Location{ .pos=<pos>, .line=<line>, .col=<col>},
            .literal = String_View{ &prog[<pos>], <len> }
        }
        """
        return "Token{{ {}, {{ {}, {}, {} }}, {{ &prog[{}], {} }} }}".format(
            self.kind, 
            self.pos, self.line, self.col, 
            self.pos, self.literal_len
        )

    def lit(self, prog: str) -> str:
        """return a representation of the literal the token represents.
        """
        return prog[self.pos:self.pos + self.literal_len]


@dataclass
class Lexer:
    prog: str
    pos: int = 0
    read_pos: int = 0
    ch: str = "\0"
    line: int = 1
    col: int = 0

    def __init__(self, prog: str):
        self.prog = prog
        self.read_char()
    
    def read_char(self):
        """Worked example of lexer advance for column/line/pos recording
        foo\n     foo(col=1, line=1, pos=0)
        bar\n     bar(col=1, line=2, pos=4)
        baz\r\n   baz(col=1, line=3, pos=8)
        bag       bag(col=1, line=4, pos=13)
        """

        if self.read_pos >= len(self.prog):
            self.ch = "\0"
        else:
            if self.ch == "\n":
                self.line += 1
                self.col = 1
            elif self.ch == "\r":
                if self.peek_char() == "\n":
                    self.read_pos += 1
                self.pos += 1
                self.line += 1
                self.col = 1
            else:
                self.col += 1
            self.ch = self.prog[self.read_pos]

        self.pos = self.read_pos
        self.read_pos += 1
    
    def peek_char(self):
        if self.read_pos >= len(self.prog):
            return "\0"
        else:
            return self.prog[self.read_pos]
    
    def skip_whitespace(self):
        while self.ch.isspace():
            self.read_char()
        return

    def read_digit(self) -> (int, TokenKind):
        pos = self.pos
        dot_count = 0
        kind = None

        while (self.ch.isdigit() or self.ch == "_" or self.ch == ".") \
            and (not self.ch.isspace()) and self.ch != "\0":
            if self.ch == ".":
                dot_count += 1

            self.read_char()

        num_len = self.pos - pos
        if dot_count > 1:
            kind = TokenKind.Illeagl
        elif dot_count == 1:
            kind = TokenKind.float_lit
        else:
            kind = TokenKind.int_lit

        return num_len, kind
    
    def read_ident(self) -> int:
        pos = self.pos
        while (self.ch.isalpha() or self.ch.isdigit() or self.ch == "_") \
            and (not self.ch.isspace()) and self.ch != "\0":
            self.read_char()
        ident_len = self.pos - pos
        return ident_len

    def read_string(self) -> int:
        pos = self.pos

        while (self.ch != "\"" and self.ch != "\0"):
            if self.ch == "\\" and self.peek_char() == "\"":
                self.read_char()
            self.read_char()

        slen = self.pos - pos
        return slen

    def next_token(self) -> Token:
        self.skip_whitespace()

        t = Token(
            pos=self.pos, line=self.line, col=self.col, 
        )

        match self.ch:
            case "\0":
                t.kind = TokenKind.Eof
            
            case "(" | ")" | "{" | "}" | "[" | "]" | ";" | ":" | "," | ".":
                t.kind = DELIMITERS[self.ch]

            case "+" | "-" | "*" | "/":
                t.kind = OPERATOR[self.ch]
            
            case "=" | "!" | "<" | ">":
                s = self.prog[self.pos:self.read_pos + 1]

                tk = OPERATOR.get(s)
                if tk is not None:
                    t.kind = OPERATOR[s]
                    t.literal_len = 2
                    self.read_char()
                else:
                    t.kind = OPERATOR[self.ch]
            
            case "|" | "&":
                s = self.prog[self.pos:self.read_pos + 1]
                tk = OPERATOR.get(s)
                if tk is not None:
                    t.kind = OPERATOR[s]
                    t.literal_len = 2
                    self.read_char()
                else:
                    raise Exception(f"Invalid character: '{self.ch}' ( str = \"{s}\")")

            case _:
                if self.ch.isdigit():
                    num_len, kind = self.read_digit()
                    t.kind = kind
                    t.literal_len = num_len
                    return t

                elif self.ch.isalpha():
                    ident_len = self.read_ident()
                    t.literal_len = ident_len
                    word = self.prog[t.pos:ident_len + 1]
                    print("\t", word)
                    kind = KEWYORDS_BUILTIN.get(word)
                    if kind is not None:
                        t.kind = kind
                    else:
                        t.kind = TokenKind.ident
                    return t

                elif self.ch == "\"":
                    self.read_char()
                    t = Token(pos=self.pos, line=self.line, col=self.col)
                    slen = self.read_string()
                    t.kind = TokenKind.stirng_lit
                    t.literal_len = slen
                    self.read_char()
                    return t
                
                else:
                    t.kind = TokenKind.Illeagl

        self.read_char()

        return t


# content = "+ - =="
l = Lexer(content)

tokens = []
while True:
    tokens.append(l.next_token())
    if tokens[-1].kind == TokenKind.Eof:
        break


for t in tokens:
    print(f"( \"{t.lit(content)}\" )", t)
