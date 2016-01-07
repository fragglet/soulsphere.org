#!/usr/bin/env python2.4
#
# Copyright(C) Simon Howard
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
# IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#
# Hexadecimal calculator, based on the PLY example calculator.
#

tokens = (
    'HEXNUMBER2','HEXNUMBER', 'DECNUMBER',
    'PLUS','MINUS','TIMES','DIVIDE',
    'AND','OR','XOR','INVERSE',
    'LSHIFT','RSHIFT',
    'LPAREN','RPAREN',
    'NAME','EQUALS'
    )

# Tokens

t_PLUS    = r'\+'
t_MINUS   = r'-'
t_TIMES   = r'\*'
t_DIVIDE  = r'/'
t_LPAREN  = r'\('
t_RPAREN  = r'\)'
t_AND     = r'\&'
t_OR      = r'\|'
t_XOR     = r'\^'
t_INVERSE = r'\~'
t_LSHIFT  = r'<<'
t_RSHIFT  = r'>>'
t_NAME    = r'\$[a-zA-Z_][a-zA-Z0-9_]*'
t_EQUALS  = r'='

# Hexadecimal numbers, plain or C-style (0x) form.

def t_HEXNUMBER(t):
    r'0x[0-9a-fA-F]+'

    t.value = int(t.value[2:], 16)

    return t

def t_HEXNUMBER2(t):
    r'[0-9a-fA-F]+'

    t.value = int(t.value, 16)

    return t

# Decimal numbers have 0n as a prefix

def t_DECNUMBER(t):
    r'n[0-9]+'

    t.value = int(t.value[1:])

    return t

# Ignored characters
t_ignore = " \t"

def t_newline(t):
    r'\n+'
    t.lexer.lineno += t.value.count("\n")

def t_error(t):
    raise RuntimeError("Illegal character '%s'" % t.value[0])

# Build the lexer
import ply.lex as lex
lex.lex()

# Parsing rules

precedence = (
    ('left','EQUALS'),
    ('left','OR'),
    ('left','XOR'),
    ('left','AND'),
    ('left','LSHIFT','RSHIFT'),
    ('left','PLUS','MINUS'),
    ('left','TIMES','DIVIDE'),
    ('right', 'INVERSE'),
    ('right','UMINUS'),
    )

# Dictionary of variables
variables = { }

def p_statement_expr(t):
    'statement : expression'
    if t[1] >= 0:
        print "0x%x" % t[1]
    else:
        print "-0x%x" % (-t[1])

def p_statement_assign(t):
    'statement : NAME EQUALS expression'
    variables[t[1]] = t[3]
    t[0] = t[3]

def p_expression_binop(t):
    '''expression : expression PLUS expression
                  | expression MINUS expression
                  | expression TIMES expression
                  | expression DIVIDE expression
                  | expression AND expression
                  | expression OR expression
                  | expression XOR expression
                  | expression LSHIFT expression
                  | expression RSHIFT expression'''
    if t[2] == '+'  : t[0] = t[1] + t[3]
    elif t[2] == '-': t[0] = t[1] - t[3]
    elif t[2] == '*': t[0] = t[1] * t[3]
    elif t[2] == '/': t[0] = t[1] / t[3]
    elif t[2] == '&': t[0] = t[1] & t[3]
    elif t[2] == '|': t[0] = t[1] | t[3]
    elif t[2] == '^': t[0] = t[1] ^ t[3]
    elif t[2] == '<<': t[0] = t[1] << t[3]
    elif t[2] == '>>': t[0] = t[1] >> t[3]

def p_expression_inverse(t):
    'expression : INVERSE expression %prec INVERSE'
    t[0] = ~t[2]

def p_expression_uminus(t):
    'expression : MINUS expression %prec UMINUS'
    t[0] = -t[2]

def p_expression_group(t):
    'expression : LPAREN expression RPAREN'
    t[0] = t[2]

def p_expression_number(t):
    '''expression : DECNUMBER
                  | HEXNUMBER
                  | HEXNUMBER2'''
    t[0] = t[1]

def p_expression_name(t):
    'expression : NAME'

    if not variables.has_key(t[1]):
        raise RuntimeError("Unknown variable '%s'" % t[1])
    else:
        t[0] = variables[t[1]]

def p_error(t):
    if t != None:
        raise RuntimeError("Syntax error at '%s'" % t.value)
    else:
        raise RuntimeError("Syntax error at end of line")

import ply.yacc as yacc
yacc.yacc()

while True:
    try:
        s = raw_input('hc> ')
        if len(s) > 0:
            yacc.parse(s)
    except RuntimeError, e:
        print e.args[0]
    except EOFError:
        break

