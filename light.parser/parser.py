from parse.tree import *
from sys import setrecursionlimit
setrecursionlimit(1000000)


class SyntaxTree:
    def __init__(self, operand: Sign, *args):
        self.operand = operand
        self.args = args

    def print(self):
        print('(', self.operand.value, ',')


class Parser:
    def __init__(self, target):
        self.target = target
        self.pos = 0
        self.token = ''

    def doall(self):
        self.__next_token()
        return self.__expression()

    def __next_token(self):
        self.token = ''
        if self.target[self.pos] in Alpha:
            while self.target[self.pos] in Alpha or self.target[self.pos] in Digit or self.target[self.pos] == '\'':
                self.token += self.target[self.pos]
                self.pos += 1
        else:
            for s in Sign:
                if self.target[self.pos:self.pos + len(s.value)] == s.value:
                    self.pos += len(s.value)
                    self.token = s
                    break
        return self.token

    def __expression(self):
        node = self.__disjunction()
        if self.token == Sign.imp:
            self.__next_token()
            node = MultiNode(Sign.imp, node, self.__expression())
        return node

    def __disjunction(self):
        node = self.__conjunction()
        while self.token == Sign.dis:
            self.__next_token()
            node = MultiNode(Sign.dis, node, self.__conjunction())
        return node

    def __conjunction(self):
        node = self.__negation()
        while self.token == Sign.con:
            self.__next_token()
            node = MultiNode(Sign.con, node, self.__negation())
        return node

    def __negation(self):
        if self.token == Sign.neg:
            self.__next_token()
            node = NegateNode(self.__negation())
        else:
            if self.token == Sign.opb:
                self.__next_token()
                node = self.__expression()
            else:
                node = VariableNode(self.token)
            self.__next_token()
        return node


line = input() + Sign.fin.value
Parser(line).doall().print()
