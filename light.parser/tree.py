from enum import Enum


class Sign(Enum):
    neg = '!'
    con = '&'
    dis = '|'
    imp = '->'
    fin = '#'
    opb = '('
    clb = ')'


Alpha = set(map(chr, range(ord('A'), ord('Z') + 1)))
Digit = set(map(chr, range(ord('0'), ord('9') + 1)))


class SyntaxTreeNode:
    def print(self):
        pass


class MultiNode(SyntaxTreeNode):
    def __init__(self, sign: Sign, *args):
        self.sign = sign
        self.children = args

    def print(self):
        print(Sign.opb.value, self.sign.value, sep='', end='')
        for child in self.children:
            print(',', end='')
            child.print()
        print(Sign.clb.value, end='')


class NegateNode(SyntaxTreeNode):
    def __init__(self, child: SyntaxTreeNode):
        self.child = child

    def print(self):
        print(Sign.opb.value, Sign.neg.value, sep='', end='')
        self.child.print()
        print(Sign.clb.value, end='')


class VariableNode(SyntaxTreeNode):
    def __init__(self, name):
        self.name = name

    def print(self):
        print(self.name, end='')


if __name__ == '__main__':
    MultiNode(
        Sign.imp,
        NegateNode(VariableNode('A')),
        MultiNode(
            Sign.con,
            NegateNode(VariableNode('B')),
            VariableNode('X')
        )
    ).print()
