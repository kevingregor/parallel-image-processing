import ctypes
from ctypes import cdll
import numpy as np
import cv2
import argparse

# Mac OS X
stdc = cdll.LoadLibrary("libc.dylib")
stdcpp = cdll.LoadLibrary("libstdc++.dylib")

# Linux
# stdc = cdll.LoadLibrary("libc.o.6")
# stdcpp = cdll.LoadLibrary("libstdc++.o.6")

# Load Library
library = cdll.LoadLibrary("./image_proc.so")

# Define pointer for double array
c_double_p = ctypes.POINTER(ctypes.c_double)

# Define class - This is necessary to make sure no auto garbage collection takes place
class CppObj(object):
	def __init__(self, image, output):
	    library.CreateInstanceOfImage.restype = ctypes.c_void_p
	    self.image_cpp = library.CreateInstanceOfImage(
	    				image.ctypes.data_as(c_double_p), 
	    				ctypes.c_int(np.shape(image)[0]), 
	    				ctypes.c_int(np.shape(image)[1]), 
	    				output.ctypes.data_as(c_double_p))

	# def callTest(self):
	# 	return library.CallMemberTest(ctypes.c_void_p(self.image_cpp))


# How To Call: python callLibrary.py <input_image> <output_image>
if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='Parallel Image Processing')
	parser.add_argument('image_file', help='Image File')
	parser.add_argument('output_file', help='Output File')
	args = parser.parse_args()

	input_img = cv2.imread(args.image_file, 0).astype(np.double) # Convert image to grayscale matrix
	output_img = cv2.imread(args.image_file, 0).astype(np.double)
	obj = CppObj(input_img, output_img) # Create the C++ IMAGE object

	# vvv -Do Whatever Operations Here- vvv #

	print obj

	# ^^^ -Do Whatever Operations Here- ^^^ #

	cv2.imwrite(args.output_file, output_img) # Save output image