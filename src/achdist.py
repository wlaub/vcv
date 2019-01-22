from matplotlib import pyplot as plt
import random
import math

def sigm(k, u=None):
    """
    u in (-1,1)
    x in (-10, 10)
    u = L/(1+e^(-kx) - L/2

    -1/L = 1/(1+e^(10k)) - 1/2
    1/L = 1/2 - 1/(1+e^(10k))

    u/L + 1/2 = 1/(1+e^(-kx))
    (u+L)/(2L) = ...
    1 + e^(-kx) = 2L/(u+L)
    e^(-kx) = 2L/(u+L) -1

    """
    L = 1/(.5-1/(1+math.exp(10*k)))
    if u == None:
        u = random.uniform(-1,1)

#    print(u, L, u/L, 1- 1/(u/L+1/2))
#    print(u, L, u/L, 2*L/(u+L)+1)

    try:
        return -(1/k)*math.log((L+2*u)/(L-2*u))
    except: raise

def isigm(k, x):
    L = 1/(.5-1/(1+math.exp(10*k)))

    return L/(1+math.exp(-k*x))-L/2



M = 4
N = 10000

"""
for idx in range(M):
    uvals = [-10+20*x/N for x in range(N)]

    vals = [isigm(.1*(idx+1), u) for u in uvals]

    plt.plot(uvals, vals)

plt.show()

exit()
"""
fig, axes = plt.subplots(M,M)
for idx in range(M*M):
    ax = axes[idx%M][int(idx/M)]
    uvals = [-1+2*x/N for x in range(N)]

    k = .01+1*idx/(M*M)

    vals = [sigm(k) for u in uvals]

    ax.set_xlim([-10, 10])
    ax.hist(vals, bins=int(N/100))
    ax.set_title(str(k))

plt.show()

