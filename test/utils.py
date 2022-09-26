import numpy as np

def write_solution(data, filename):
    with open(filename, mode='w') as fout:
        fout.write('\n'.join(' '.join(str(x) for x in row) for row in data))


def read_solution(filename):
    with open(filename) as fin: 
        parsed_input = [[float(x) for x in line.strip().split()] for line in fin]
    return np.array(parsed_input)