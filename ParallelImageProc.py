import ctypes
from ctypes import cdll
import numpy as np
# import cv2
import argparse
from PIL import Image

# Mac OS X
#stdc = cdll.LoadLibrary("libc.dylib")
#stdcpp = cdll.LoadLibrary("libstdc++.dylib")

# Linux
# stdc = cdll.LoadLibrary("libc.o.6")
# stdcpp = cdll.LoadLibrary("libstdc++.o.6")

# Load Library
library = cdll.LoadLibrary("./libimageproc.so")

# Define class - This is necessary to make sure no auto garbage collection takes place
class CppObj(object):
    def __init__(self, image, Format, layout):
        library.create_image_instance.restype = ctypes.c_void_p
        self.image_cpp = library.create_image_instance(image.ctypes.data_as(ctypes.c_void_p), ctypes.c_int(np.shape(image)[1]), ctypes.c_int(np.shape(image)[0]), Format, layout)

    def perform_filtering(self, Filter):
        library.perform_filtering.restype = ctypes.c_double
        return library.perform_filtering(ctypes.c_void_p(self.image_cpp), Filter)

    def get_processed_image(self, output):
        library.get_processed_image(ctypes.c_void_p(self.image_cpp), output.ctypes.data_as(ctypes.c_void_p))

    def destroy_image_instance(self):
        library.destroy_image_instance(ctypes.c_void_p(self.image_cpp))

# Mimic the enum for FORMAT
def matchFormat(Format):
    if Format == "L8":
        return ctypes.c_int(0)
    elif Format == "L16":
        return ctypes.c_int(1)
    elif Format == "L32":
        return ctypes.c_int(2)
    elif Format == "RGB888":
        return ctypes.c_int(3)
    elif Format == "RGBX8888":
        return ctypes.c_int(4)
    else:
        return ctypes.c_int(-1)

# Mimic the enum for LAYOUT
def matchLayout(layout):
    if layout == "STRIDED":
        return ctypes.c_int(0)
    elif layout == "BLOCK_LINEAR_8":
        return ctypes.c_int(1)
    elif layout == "BLOCK_LINEAR_16":
        return ctypes.c_int(2)
    elif layout == "BLOCK_LINEAR_32":
        return ctypes.c_int(3)
    elif layout == "TWIDDLED":
        return ctypes.c_int(4)
    else:
        return ctypes.c_int(-1)

# Mimic the enum for FILTER_TYPE
def matchFilter(Filter):
    if Filter == "BOX_FILTER":
        return ctypes.c_int(0)
    elif Filter == "GAUSSIAN_BLUR":
        return ctypes.c_int(1)
    elif Filter == "EDGE_DETECT":
        return ctypes.c_int(2)
    else:
        return ctypes.c_int(-1)


# How To Call: python ParallelImageProc.py <input_image> <image_format> <layout_tobe_used> <filter>
# Example: python ParallelImageProc.py img.jpg L8 STRIDED BOX_FILTER
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Parallel Image Processing')
    parser.add_argument('image_file', help='Image File')
    parser.add_argument('image_format', help='Image Format (RGB888, L8, etc)')
    parser.add_argument('layout_tobe_used', help='Memory Layout to be used')
    parser.add_argument('filter', help='Filter to be used')
    args = parser.parse_args()

    img = Image.open(args.image_file)
    input_img = np.array(img)
    input_img = input_img[:,:,::-1]

    # initializing so that the dimensions match
    processed_output = np.array(img)
    processed_output = processed_output[:,:,::-1]

    
    total_time = 0.0
    num_iters = 5
    for i in range(num_iters):
        obj = CppObj(input_img, matchFormat(args.image_format), matchLayout(args.layout_tobe_used)) # Create the C++ IMAGE object
        time = obj.perform_filtering(matchFilter(args.filter))
        total_time += time
        obj.get_processed_image(processed_output)
        obj.destroy_image_instance()

    print (args.image_file + " - " + args.layout_tobe_used + " " + args.filter + ": " + str(total_time/num_iters))



    processed_output = processed_output[:,:,::-1]
    Image.fromarray(processed_output).save('processed' + args.image_file)
