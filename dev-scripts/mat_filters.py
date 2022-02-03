import sys, os
import json
import math

import scipy.signal
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import widgets as mplwidgets
from matplotlib import colors as mplcolors

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

    def gain(self, g):
        self.b = [x*g for x in self.b]

    def freqz(self, **kwargs):
        fs = kwargs.pop('fs', self.fs)
        w,h = scipy.signal.freqz(self.b, self.a, fs=fs, **kwargs)
        h**=self.N
#        self.normalize()        
#        sos = np.array([[*self.b, *self.a]])

#        w,h = scipy.signal.sosfreqz(sos, fs = fs, **kwargs)
        return w,h

    def plot(self, ax):
#        for fc in self.fc:
#            ax.axvline(fc, c='k')
        pass

class LPF(Filter):
    def __init__(self, fs, fc):
        super().__init__(fs)
        self.N = 4
        self.compute_lpf(fc)

class HPF(Filter):
    def __init__(self, fs, fc):
        super().__init__(fs)
        self.N = 4
        self.compute_hpf(fc)

class BPF(Filter):
    def __init__(self, fs, fl, fr):
        super().__init__(fs)
        self.N = 4
        self.compute_bpf(fl, fr)



def compute_filters(fs, **kwargs):
    N = 5           #Number of bandpass bands - must be odd
    ref = 261.63*2    #Center band (hehehehe) center frequency (HAHAHAHA)
    bw = kwargs.get('bw', 2)
    base = ref * 2**(-N*bw/2)

    filters = []

    bulge = kwargs.get('bulge', 0)
    lb = kwargs.get('low_bulge', 0)
    hb = kwargs.get('high_bulge', 0)
    gbpf = kwargs.get('boost', 1)

    filters.append(LPF(fs, base*2**(lb)))
    for i in range(N):
        fc = base*2**(bw*(i+.5))
        scale = 2**((bw+bulge)/2)
        fl = fc/scale
        fr = fc*scale

        filt = BPF(fs, fl, fr)
        if i > 0 and i < N-1:
            filt.gain(gbpf)

        filters.append(filt)

    filters.append(HPF(fs, fr/2**(hb)))

    return filters

def plot_filters(filters, fig, ax):

    fs = filters[0].fs

    haccum = None

    f = []
    N = 512
    fmin = 1
    fmax = fs/2
    delta = (fmax/fmin)**(1/N)
    f = [fmin*(fmax/fmin)**(n/N) for n in range(N)]

    colors = [
        [1,0,0],
        [1,.5,0],
        [1,1,0],
        [0,1,0],
        [0,1,1],
        [0,0,1],
        [.5,0,1]
        ]

    for idx, filt in enumerate(filters):
        w,h = filt.freqz(worN=f)

        ax.loglog(w, np.abs(h), linewidth = 1, c = colors[idx])
        filt.plot(ax)

        if haccum is None:
            haccum = h
        else:
            haccum += h

    ax.loglog(w, np.abs(haccum), linewidth = 1, c='k')

    ax.axhline(math.sqrt(2)/2, c='k')

    ax.set_ylim(bottom = 10e-3, top= 2)
    ax.set_xlim(right = fs/2, left=1)
    ax.grid(True)

def dump_filters(filters):
    spec = {}

    fs = filters[0].fs
    filter_array = []
    for filt in filters:
        filter_array.append(filt.to_json())
        
    spec['fs'] = fs
    spec['channels'] = filter_array
    return spec


def dump_spec(fses, ckwargs):
    data = []
    result = {'filters': data}
    for fs in fses:
        filters = compute_filters(fs, **ckwargs)
        spec = dump_filters(filters)
        data.append(spec)
    return result


class PlotHandler():

    param_defaults = {
        'low_bulge': ['Low', -1, 2, .583, True],
        'bulge': ['Bulge', 0, 2, 0.55, True],
        'high_bulge': ['High', -1, 2, .693, True],
        'boost': ['Boost', 1, 2, 1, False],
        'bw': ['BW', 1, 2, 2, True],
        }

    def __init__(self):
        self.fig, self.ax = plt.subplots()

        self.params = {k:v[3] for k,v in self.param_defaults.items()}

        actives = list(filter(lambda x: x[-1], self.param_defaults.items()))

        count = len(actives)
        gap = 0.08/count        

        idx = 0
        self.sliders = []
        for k, (label, vmin, vmax, vdef, enable) in actives:
            if not enable: continue
            sliderax = plt.axes([0.92+gap*idx, .05, .01, .9])
            idx += 1

            slider = mplwidgets.Slider(sliderax, label, 
                valmin =vmin, valmax = vmax, valinit = vdef,
                orientation='vertical'
                )            
            slider.on_changed(lambda x, k=k: self.update(k, x))
            self.sliders.append(slider)

        self.buttons = []
        buttax = plt.axes([0.5-.0125, .95, .05, .025])
        button = mplwidgets.Button(buttax, 'Export')
        button.on_clicked(self.export)
        self.buttons.append(button)

    def update(self, label, x):
        self.params[label] = x
        self.plot()


    def plot(self):

        fs = 48000
        filters = compute_filters(fs, **self.params)
        self.ax.clear()
        plot_filters(filters, self.fig, self.ax)

        ticks = []
        mticks = []
        tick_labels = []
        for i in range(-10,10):
            ticks.append(32.7*2**i)
            tick_labels.append(f'C{i}')            
            for j in range(12):
                mticks.append(32.7*2**(i+j/12))
        self.ax.set_xticks(ticks, labels=tick_labels)
        self.ax.set_xticks(mticks, labels = [], minor=True)
        self.ax.set_xlim(1, fs/2)
    
        self.ax.axvline(20, c='k')

        self.ax.text(20, 2.1, '20 Hz', horizontalalignment='center')
        self.ax.text(.9, .707, '-3 dB', horizontalalignment='right', verticalalignment='center')

        def format_coord(x,y):
            y = 20*math.log10(y)
            return f'{x:.2f} Hz, {y:.2f} dB'

        self.ax.format_coord = format_coord
        



    def export(self, x):
        filename = 'test.json'
        if len(sys.argv) > 1:
            filename = sys.argv[1]

        fses = []
        for x in [1,2,4]:
            fses.extend([44.1e3*x, 48e3*x])

        data = dump_spec(fses, self.params)
        with open(filename, 'w') as fp:
            json.dump(data, fp)

        print(f'Exported {filename}')


handler = PlotHandler()
handler.plot()
plt.show()





