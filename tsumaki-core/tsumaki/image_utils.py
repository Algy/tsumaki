import numpy as np

from PIL import Image


def resize(arr, wh):
    t  = Image.fromarray(arr)
    g = t.resize(wh)
    return np.array(g)

