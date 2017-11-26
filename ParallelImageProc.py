import ctypes
from ctypes import cdll
import numpy as np
import cv2
import argparse

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

    def __init__(self, image):

        library.create_image_instance.restype = ctypes.c_void_p
        self.image_cpp = library.create_image_instance(image.ctypes.data_as(ctypes.c_void_p), ctypes.c_int(np.shape(image)[1]), ctypes.c_int(np.shape(image)[0]))

    def perform_filtering(self):
        #library.perform_filtering.restype = ctypes.c_void
        library.perform_filtering(self.image_cpp)

    def get_processed_image(self, output):
        #library.get_processed_image.restype = ctypes.c_void
        library.get_processed_image(self.image_cpp, output.ctypes.data_as(ctypes.c_void_p))

    def destroy_image_instance(self):
        #library.destroy_image_instance.restype = ctypes.c_void
        library.destroy_image_instance(self.image_cpp)


# How To Call: python ParallelImageProc.py <input_image> <image_format> <layout_tobe_used> <filter>
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Parallel Image Processing')
    parser.add_argument('image_file', help='Image File')
    """
    parser.add_argument('image_format', help='Image Format (RGB888, L8, etc)')
    parser.add_argument('layout_tobe_used', help='Memory Layout to be used')
    parser.add_argument('filter', help='Filter to be used')
    """
    args = parser.parse_args()

    input_img = cv2.imread(args.image_file, cv2.IMREAD_COLOR)

    # initializing so that the dimensions match
    processed_output = cv2.imread(args.image_file, cv2.IMREAD_COLOR)

    obj = CppObj(input_img) # Create the C++ IMAGE object

    obj.perform_filtering()

    obj.get_processed_image(processed_output)

    obj.destroy_image_instance()

    cv2.imwrite('processed' + args.image_file, processed_output) # Save output image

