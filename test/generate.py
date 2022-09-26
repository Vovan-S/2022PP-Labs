import numpy as np
from visualize import plot_wave

def first_test(P, tau, M, steps):
    h = 3 / M
    T = P * tau 
    res = np.zeros((P, M))
    t = np.linspace(
        np.zeros((M,)),
        np.full((M,), T),
        P,
        axis=0
    )
    x = np.linspace(
        np.zeros((P,)),
        np.full((P,), 3),
        M,
        axis=1
    )
    for n in range(1, steps + 1):
        res += 18 / np.pi / n * np.cos(np.pi * n * t) * np.sin(np.pi * n * x / 3)
    return res

x = first_test(300, 0.1, 100, 100)

plot_wave(x, 3 / 100, 'test_wave.gif')