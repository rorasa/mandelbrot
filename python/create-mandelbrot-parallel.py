import numpy as np
from PIL import Image
from tqdm import tqdm
import sys
from multiprocessing import Pool
from functools import partial

def TestMandelbrotMembership(c, limit=1000):

    z = 0
    for i in range(0, limit):
        z = (z*z) + c
        if abs(z) > 2:
            return False
    
    return True

def CreateMandelbrotLine(limit, re_resolution, im_resolution, length, index):
    mandelbrot_line = np.zeros(length, dtype=np.uint8)
    if index % 10 == 0:
        print("Computing index {}".format(index))
        
    for j in range(0, length):
        re = ( index / re_resolution) + limit["re_low"]
        im = ( j / im_resolution) + limit["im_low"]
        if TestMandelbrotMembership( complex(re,im) ):
            mandelbrot_line[j] = 1

    return mandelbrot_line

def CreateMandelbrotSet(size):

    pool_size = 4

    limit = {
        "re_low": -0.5,
        "re_high": 0,
        "im_low": -1,
        "im_high": -0.5
    }

    #low_limit_re = -2
    #high_limit_re = 2
    #low_limit_im = -2
    #high_limit_im = 2

    re_resolution = size/(limit["re_high"] - limit["re_low"])
    im_resolution = size/(limit["im_high"] - limit["im_low"])

    LineFunction = partial(CreateMandelbrotLine, limit, re_resolution, im_resolution, size)

    with Pool(pool_size) as p:
        lines = p.map(LineFunction, tqdm(range(0, size)))

    return np.stack(lines, axis=1)
    
def MakeImage(mandelbrot_set):
    img = Image.fromarray(mandelbrot_set*255, "L")
    img.save('output.png')

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("No image size given")
        sys.exit(-1)

    print("Creating mandelbrot set")
    image_size = int(sys.argv[1])
    if not (image_size % 4) == 0:
        print("Warning, image_size should be dividable by 4")

    mandelbrot_set = CreateMandelbrotSet(image_size)
    MakeImage(mandelbrot_set)

