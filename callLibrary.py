from ctypes import cdll

# Mac OS X
stdc = cdll.LoadLibrary("libc.dylib")
stdcpp = cdll.LoadLibrary("libstdc++.dylib")

# Linux
# stdc = cdll.LoadLibrary("libc.o")
# stdcpp = cdll.LoadLibrary("libstdc++.o")

library = cdll.LoadLibrary("./test.so")

inst = library.CreateInstanceOfClass()

print (library.CallMemberTest(inst))