import numpy as np
from PIL import Image
from tqdm import tqdm
import sys

def TestMandelbrotMembership(c, limit=1000):

    z = 0
    for i in range(0, limit):
        z = (z*z) + c
        if abs(z) > 2:
            return False
    
    return True

def CreateMandelbrotSet(size):

    mandelbrot_set = np.zeros([size, size], dtype=np.uint8)

    low_limit_re = -2
    high_limit_re = 2
    low_limit_im = -2
    high_limit_im = 2

    re_resolution = size/(high_limit_re - low_limit_re)
    im_resolution = size/(high_limit_im - low_limit_im)
    
    for i in tqdm(range(0, size)):
        for j in range(0, size):

            re = ( i / re_resolution) + low_limit_re
            im = ( j / im_resolution) + low_limit_im

            if TestMandelbrotMembership( complex(re,im) ):
                mandelbrot_set[j][i] = 1

    return mandelbrot_set

def MakeImage(mandelbrot_set):
    img = Image.fromarray(mandelbrot_set*255, "L")
    img.save('output.png')


if len(sys.argv) < 2:
    print("No image size given")
    sys.exit(-1)

print("Creating mandelbrot set")
image_size = int(sys.argv[1])
if not (image_size % 4) == 0:
    print("Warning, image_size should be dividable by 4")

mandelbrot_set = CreateMandelbrotSet(image_size)
MakeImage(mandelbrot_set)






