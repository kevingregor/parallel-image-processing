
CXX = g++
CXX_FLAGS = -O3 -fPIC -fopenmp
# CXX_FLAGS = -O3 -fPIC
LINK_FLAGS = -shared


all : libimageproc.so

libimageproc.so : image.o image_proc.o
	$(CXX) $^ $(CXX_FLAGS) $(LINK_FLAGS) -o $@

image.o : image.cpp image.h image_proc.h
	$(CXX) -c $< $(CXX_FLAGS) -o $@

image_proc.o : image_proc.cpp image.h image_proc.h
	$(CXX) -c $< $(CXX_FLAGS) -o $@

clean:
	rm -f *.o *.so
