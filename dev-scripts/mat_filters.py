import sys, os
import json
import math

import scipy.signal
import numpy as np
from matplotlib import pyplot as plt

class Filter():
    def __init__(self, fs):
        self.fs = fs
        self.N = 1

    def compute_bpf(self, fl, fr):
        fs = self.fs

        bw = math.log(fr/fl)/math.log(2)
        fc = fl*2**(bw/2)

        wc = 2*math.pi*fc/fs
        alpha = math.sin(wc)*math.sinh((math.log(2)/2)*(bw)*(wc/math.sin(wc)))
        self.b = b = [alpha, 0, -alpha]
        self.a = a = [1+alpha, -2*math.cos(wc), 1-alpha]
        self.fc = [fl, fr]
        return b,a

    def compute_lpf(self, fc):
        fs = self.fs
        wc = 2*math.pi*fc/fs
        alpha = math.sin(wc)*math.sqrt(2)/2
        self.b = b = [(1-math.cos(wc))/2, 1-math.cos(wc), (1-math.cos(wc))/2]
        self.a = a = [1+alpha, -2*math.cos(wc), 1-alpha]
        self.fc = [fc]
        return b,a

    def compute_hpf(self, fc):
        fs = self.fs
        wc = 2*math.pi*fc/fs
        alpha = math.sin(wc)*math.sqrt(2)/2
        self.b = b = [(1+math.cos(wc))/2, -1-math.cos(wc), (1+math.cos(wc))/2]
        self.a = a = [1+alpha, -2*math.cos(wc), 1-alpha]
        self.fc = [fc]
        return b,a

    def to_json(self):
        """
        Return a list of sos 
        [[b0, b1, b2, a0, a1, a2], ...]
        """
        return [[*self.b, *self.a]]*self.N

    def normalize(self):
        b,a = self.b[0], self.a[0]
        self.b = [x/b for x in self.b]
        self.a = [x/a for x in self.a]

    def freqz(self, **kwargs):
        fs = kwargs.pop('fs', self.fs)
        w,h = scipy.signal.freqz(self.b, self.a, fs=fs, **kwargs)
        h**=self.N
#        self.normalize()        
#        sos = np.array([[*self.b, *self.a]])

#        w,h = scipy.signal.sosfreqz(sos, fs = fs, **kwargs)
        return w,h

    def plot(self, ax):
        for fc in self.fc:
            ax.axvline(fc, c='k')

class LPF(Filter):
    def __init__(self, fs, fc):
        super().__init__(fs)
        self.compute_lpf(fc)

class HPF(Filter):
    def __init__(self, fs, fc):
        super().__init__(fs)
        self.compute_hpf(fc)

class BPF(Filter):
    def __init__(self, fs, fl, fr):
        super().__init__(fs)
        self.N = 4
        self.compute_bpf(fl, fr)



def compute_filters(fs):
    N = 5           #Number of bandpass bands - must be odd
    ref = 261.63*2    #Center band (hehehehe) center frequency (HAHAHAHA)
    bw = 2
    base = ref * 2**(-N*bw/2)

    filters = []

    filters.append(LPF(fs, base))
    for i in range(N):
        fl = base*2**(bw*i)
        fr = fl*2**(bw)

        filters.append(BPF(fs, fl, fr))

    filters.append(HPF(fs, fr))

    return filters

def plot_filters(filters):

    fs = filters[0].fs

    haccum = None

    fig, ax = plt.subplots()

    for filt in filters:
        w,h = filt.freqz(worN=2048)

        ax.loglog(w, np.abs(h), linewidth = 0.5)
        filt.plot(ax)

        if haccum is None:
            haccum = h
        else:
            haccum += h

    ax.loglog(w, np.abs(haccum), linewidth = 1, c='k')

    ax.axhline(math.sqrt(2)/2, c='k')

    ax.set_ylim(bottom = 10e-3)
    ax.set_xlim(right = fs/2)
    ax.grid(True)

    plt.show()

def dump_filters(filters):
    spec = {}

    fs = filters[0].fs
    filter_array = []
    for filt in filters:
        filter_array.append(filt.to_json())
        
    spec['fs'] = fs
    spec['channels'] = filter_array
    return spec


def dump_spec(fses):
    data = []
    result = {'filters': data}
    for fs in fses:
        filters = compute_filters(fs)
        spec = dump_filters(filters)
        data.append(spec)
    return result


filename = 'test.json'
if len(sys.argv) > 1:
    filename = sys.argv[1]

data = dump_spec([44100, 48000])
with open(filename, 'w') as fp:
    json.dump(data, fp)

fs = 48000
filters = compute_filters(fs)
plot_filters(filters)





