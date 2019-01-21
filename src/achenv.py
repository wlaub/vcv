from matplotlib import pyplot as plt


s = 1
a = 1
b = .5
res = 100
sharp = 2

def env_base(shape, mirror=False):
    #envelope half of the given shape
    shape = max(shape, -1)
    if shape > 0:
        shape *= sharp
    xvals = [x/res for x in range(0, res+1)]
    yvals = [x**(shape+1) if x != 0 else 0 for x in xvals]
    xvals = [x/2 for x in xvals]
    if mirror:
        xvals = [1-x for x in xvals][::-1]
        yvals = yvals[::-1]


    return xvals, yvals

def get_env(shape, sym):

    alpha = abs(sym)
    left_shape = -1*alpha+shape*(1-alpha)
    right_shape = 1*alpha+shape*(1-alpha)
    shapes = [left_shape, right_shape]

    if sym>0:
        shapes = shapes[::-1]

    print('\t'.join(map(str, [shape, sym, *shapes])))

    skew = .5+sym/2

    left = env_base(shapes[0])
    right = env_base(shapes[1], True)
    xvals = left[0]
    yvals = left[1]
    xvals.extend(right[0])
    yvals.extend(right[1])

    def shift(val, sym):
        if val < .5:
            return 2*val*skew
        return skew+(1-skew)*2*(val-.5) 

    xvals = [ 
        shift(x, sym)
        for x in xvals]

    return xvals, yvals


N = 5
M = 5

sN = 2/(N-1)
sM = 2/(M-1)

for i in range(N):
    ta = (i-(N-1)/2)*sN
    for j in range(M):
        try:
            tb = (j-(M-1)/2)*sM
            plt.subplot(N,M, M*(i)+ j+1)
            title=f'{ta} {tb}'
            plt.title(title)
            plt.plot(*get_env(ta,tb))
            plt.tick_params(axis='both', which='both', bottom=False, labelbottom=False, left=False, labelleft=False)
        except Exception as e: print(str(e))

plt.show()

