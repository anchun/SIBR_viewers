# I did not manage to run this simple thing
# within those funcking CMakeFiles. Now I will
# run python script instead of losing precious
# time with their shitty language.

import sys
import glob
import shutil
import os

def symlink(source, link_name):
    os_symlink = getattr(os, "symlink", None)
    if callable(os_symlink):
        os_symlink(source, link_name)
    else:
        import ctypes
        csl = ctypes.windll.kernel32.CreateSymbolicLinkW
        csl.argtypes = (ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_uint32)
        csl.restype = ctypes.c_ubyte
        flags = 1 if os.path.isdir(source) else 0
        if csl(link_name, source, flags) == 0:
            raise ctypes.WinError()


dest_dir = sys.argv[2]
src_dir = sys.argv[1]
if not os.path.exists(dest_dir):
    os.makedirs(dest_dir)
for f in glob.glob(src_dir):
    print("Copy "+f + " to "+dest_dir)
    #symlink(f, dest_dir) # doesn't work because need privileges
    shutil.copy(f, dest_dir)
