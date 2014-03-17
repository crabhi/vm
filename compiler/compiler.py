from cStringIO import StringIO

from ast import Variable

class _WriterReg(object):

    def __init__(self):
        self.writers = {}
        self.buffer = StringIO()

    def writer(self, for_type):
        def reg(function):
            self.writers[for_type] = function
            return function
        return reg

    def write(self, value):
        writer = self.writers[type(value)]
        if writer is None:
            raise Exception("No writer for type %s, value %s" % (type(value), value))

        writer(value, self.buffer)

_R = _WriterReg()

def int_writer_gen(bytes):
    overflow_val = 2 ** (bytes * 8)

    def int_writer(value, output):
        "Writes a Big Endian integer"
        if value >= overflow_val:
            raise Exception("%d doesn't fit into %d bytes" % (value, bytes))

        for i in range(bytes):
            byte = chr(value % 256)
            output.write(byte)
            value >>= 8

    return int_writer

int_32_writer = (int_writer_gen(4))
int_16_writer = int_writer_gen(2)

@_R.writer(int)
def int_writer(value, output):
    output.write("i")
    int_32_writer(value, output)

@_R.writer(str)
def str_writer(value, output):
    output.write("s")
    str_16_writer(value, output)

def str_16_writer(value, output):
    "Writes a string of length up to 65535"
    int_16_writer(len(value), output)
    output.write(value)

@_R.writer(list)
def list_writer(value, output):
    output.write("(")
    int_16_writer(len(value), output)
    for item in value:
        _R.write(item)

@_R.writer(Variable)
def variable_writer(value, output):
    output.write("v")
    str_16_writer(value.name, output)

def compile(ast):
    """AST is a value. Value is either a list, int or string. List contains values."""

    _R.write(ast)
    return _R.buffer.getvalue()
