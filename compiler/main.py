#!/usr/bin/env python

import sys
import compiler as c

from ast import * # for use in eval'd code

if __name__ == "__main__":
    # TODO parser
    ast = eval(sys.stdin.read())
    sys.stdout.write(c.compile(ast))
