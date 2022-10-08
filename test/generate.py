import numpy as np
from visualize import plot_wave
import utils


DEFAULT_VALUES = [500, 500, 0.1, 0.1, 0.5]


def generate_data(P, M, h, tau, a):
    l = M * h
    t = np.linspace(
        np.zeros((M,)),
        np.full((M,), P*tau),
        P,
        axis=0
    )
    x = np.linspace(
        np.zeros((P,)),
        np.full((P,), l),
        M,
        axis=1
    )
    return np.sin(np.pi * x / l)*np.sin(np.pi*a*t/l)


def compare_data(filename):
    actual = generate_data(*DEFAULT_VALUES)
    got = utils.read_solution(filename)
    diff = np.abs(actual - got)
    print("File:", filename)
    print("Average difference:", np.average(diff))
    print("Max difference:", np.max(diff))


if __name__ == "__main__":
    compare_data("solution/numpy/res.txt")