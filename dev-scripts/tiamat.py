import sys, os
import json
import math

import scipy.signal
import numpy as np

"""
Helpers for generating Mat I filter specifications using filters designed in
scipy.signal

In order to accodommodate multiple engine rates in VCV, Mat I uses filter
specifications that provide different implementations for different sample rates.

The Filter object groupes multiple implementations of a signal filter.

write_filter_spec writes a json file representing a list of Filter objects.

Example:

import tiamat

filters = []
for fc in [1000, 2000, 3000, 4000]:
    f = tiamat.Filter()

    for fs in [44100, 48000, 96000]:
        sos = scipy.signal.butter(6, fc, fs=fs, output='sos')
        f.add_filter(fs, sos)

    filters.append(f)

tiamat.write_filter_spec(filters, 'filter_export_test.json')
"""

class Filter():
    """
    Helper class for grouping and exporting multiple implementations of a filter
    designed using scipy.signal.
    """    

    def __init__(self):
        self.filters = {}

    def add_filter(self, fs, sos=None, ba=None, zpk=None):
        """
        Add a filter implementation with the given sample rate. Only one
        implementation may be added per sample rate.
        
        Any scipy filter format (sos, ba, zpk) can be used, all will be converted
        and exported as sos, so it's best to use native sos.
        """

        if fs in self.filters.keys():
            raise KeyError(f"There is already a filter for {fs=}")

        nones = len(list(filter(lambda x: x is None, [sos, ba, zpk])))
        if nones != 2:
            raise RuntimeError("Must provide exactly one of sos, ba, zpk")

        if ba is not None:
            sos = scipy.signal.tf2sos(*ba)
        elif zpk is not None:
            sos = scipy.signal.zpk2sos(*zpk)

        self.filters[fs] = sos


    def export(self, fs):
        sos = self.filters[fs]

        result = []
        for filt in sos:
            result.append(list(filt))
            
        return result

    def get_fses(self):
        return list(self.filters.keys())

       
def export_filters(filters):
    """
    export a list of filter objects in the format of a Mat I filter
    specification.
    """
    result = []
    fses = sorted(filters[0].get_fses())

    for fs in fses:
        filter_array = []
        group_spec = {'fs': fs, 'channels': filter_array}
        for filt in filters:
            filter_array.append(filt.export(fs))
        result.append(group_spec)

    result = {'filters':result}
    return result

def write_filter_spec(filters, filename):
    """
    Write a Mat I filter spect with the specified filename for the given list
    of Filter objects
    """
    data = export_filters(filters)
    with open(filename, 'w') as fp:
        json.dump(data, fp, indent = 4)




