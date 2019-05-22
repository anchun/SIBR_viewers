# --------------------------------------------
""" @package ulr_preprocess
This script creates a SIBR template dataset from the old SIBR dataset which can be fed to a SIBR application

Parameters: -h help,
            -i <path to input directory which is the output from RC> <default: ${CMAKE_INSTALL_DIR}/bin/datasets/rc_out/>,
            -d <path to output directory which can be fed into SIBR apps> <default: input directory> [optional],
            -r use release w/ debug symbols executables

Usage: python ibr_preprocess_rc_to_sibr.py -i <path_to_sibr>\sibr\install\bin\datasets\museum_sibr_old_preproc
                                           -d <path_to_sibr>\sibr\install\bin\datasets\museum_sibr_new_preproc2

"""

import subprocess
import shutil
import os
import re
from pathlib import Path

from os import walk

#--------------------------------------------

#===============================================================================

import struct
import imghdr

def get_image_size(fname):
    '''Determine the image type of fhandle and return its size.
    from draco'''
    with open(fname, 'rb') as fhandle:
        head = fhandle.read(24)
        if len(head) != 24:
            return
        if imghdr.what(fname) == 'png':
            check = struct.unpack('>i', head[4:8])[0]
            if check != 0x0d0a1a0a:
                return
            width, height = struct.unpack('>ii', head[16:24])
        elif imghdr.what(fname) == 'gif':
            width, height = struct.unpack('<HH', head[6:10])
        elif imghdr.what(fname) == 'jpeg':
            try:
                fhandle.seek(0) # Read 0xff next
                size = 2
                ftype = 0
                while not 0xc0 <= ftype <= 0xcf:
                    fhandle.seek(size, 1)
                    byte = fhandle.read(1)
                    while ord(byte) == 0xff:
                        byte = fhandle.read(1)
                    ftype = ord(byte)
                    size = struct.unpack('>H', fhandle.read(2))[0] - 2
                # We are at a SOFn block
                fhandle.seek(1, 1)  # Skip `precision' byte.
                height, width = struct.unpack('>HH', fhandle.read(4))
            except Exception: #IGNORE:W0703
                return
        else:
            return
        return width, height

#===============================================================================
# SYSTEM SETTINGS
#===============================================================================
# INSTRUCTIONS:
# 1) call 'settings = load_settings("<your_filename>")'
# 2) then get value with 'get_settings(settings, "<your-key>")' whenever
# you want.

import sys, getopt

def _load_settings_keyvalue(line):
    """Return a list with 2 elements: a key and a value.
    Return an empty list if this line doesn't contain such pair.
    """
    out = []
    if len(line) < 5:
        return out
    endbracket = line.find("]:")
    if line[0] == '[' and endbracket > 0:
        out.append(line[1:endbracket])
        out.append(line[endbracket+len("]:"):].strip())
    return out    

def load_settings(filename):
    """Return a dictionary of key/value settings for the given file.

    File Format:
    [your-key]: your-value
    # a commented line
    """
    settings_dict = {}
    f = open(settings_filename, 'r')
    for line in f:
        pair = _load_settings_keyvalue(line[:-1])
        if len(pair) == 2: # else, it is either a comment or an error
            settings_dict[pair[0]] = pair[1]
    return settings_dict 

def get_settings(settings_dict, key):
    """Return the value stored in 'filename'using the given 'key'.
    Additionally, it automatically prints an error message.
    """
    if key in settings_dict:
        return settings_dict[key]
    print("ERROR: Attempting to load an unknown settings key ('" + key + "')")
    sys.exit(-1)
    return ""

    
def checkOutput( output, force_continue ):
    if( output != 0):
        if( not force_continue ):
            sys.exit()
        else:
            return False
    else:
        return True
    

#===============================================================================

settings_filename = "settings.ini"
settings = load_settings(settings_filename)

#--------------------------------------------
# 0. Paths, commands and options

def main(argv, path_dest):
    opts, args = getopt.getopt(argv, "hi:rd:", ["idir=", "bin="])
    executables_suffix = ""
    executables_folder = ""
    path_data = ""
    for opt, arg in opts:
        if opt == '-h':
            print("-i path_to_rc_data_dir -d path_to_destination_dir [-r (use release w/ debug symbols executables)]")
            sys.exit()
        elif opt == '-i':
            path_data = arg
            print(['Setting path_data to ', path_data])
        elif opt == '-d':
            path_dest = arg
            print(['Setting path_dest to ', path_dest])

    return (path_data, path_dest, executables_suffix, executables_folder)

path_dest = ""
path_data, path_dest, executables_suffix, executables_folder = main(sys.argv[1:], path_dest)

if(path_dest == ""):
    path_dest = path_data

path_data = os.path.abspath(path_data + "/") + "/"
path_dest = os.path.abspath(path_dest + "/") + "/"

path_in_imgs = path_data


print(['Raw_data folder: ', path_data])
print(['Path_dest: ', path_dest])

#path_dest_pmvs    = path_dest + "pmvs/models/";
file_nameList   = path_data + "images/list_images.txt";
# path_scene_metadata = path_data + "scene_metadata.txt"


#--------------------------------------------
# Create scene metadata file from list image file
scene_metadata = "Scene Metadata File\n\n"

# read list image file
path_list_images = os.path.join(path_in_imgs, "list_images.txt")
list_images = []

if os.path.exists(path_list_images):
    list_image_file = open(path_list_images, "r")

    for line in list_image_file:
        list_images.append(line)

    list_image_file.close()

# read clipping planes file
path_clipping_planes = os.path.join(path_data, "clipping_planes.txt")
clipping_planes = []

if os.path.exists(path_clipping_planes):
    clipping_planes_file = open(path_clipping_planes, "r")

    for line in clipping_planes_file:
        clipping_planes.append(line)

    clipping_planes_file.close()


folder_to_create = ["images","cameras","meshes","textures"]
for f in folder_to_create:
    if not os.path.exists(os.path.join(path_dest,f)):
        os.mkdir(os.path.join(path_dest,f))

scene_metadata = scene_metadata + "[list_images]\n<filename> <image_width> <image_height> <near_clipping_plane> <far_clipping_plane>\n"

for im in list_images:
    print("copying: "+im.split(' ', 1)[0])
    shutil.copy(
        Path(path_data,im.split(' ', 1)[0]),
        Path(path_dest,"images",im.split(' ', 1)[0])
        )

    if len(clipping_planes) is not 0:
        scene_metadata = scene_metadata + im[:-1] + " " + clipping_planes[0] + "\n"
    else:
        scene_metadata = scene_metadata + im[:-1] + " 0.01 100\n"

shutil.copy(
        Path(path_data,"list_images.txt"),
        Path(path_dest,"images","list_images.txt")
        )

shutil.copy(
        Path(path_data,"bundle.out"),
        Path(path_dest,"cameras","bundle.out")
        )

shutil.copy(
        Path(path_data,"pmvs/models/pmvs_recon.ply"),
        Path(path_dest,"meshes/recon.ply")
        )

scene_metadata = scene_metadata + "\n\n// Always specify active/exclude images after list images\n\n[exclude_images]\n<image1_idx> <image2_idx> ... <image3_idx>\n"


path_scene_metadata = os.path.join(path_dest, "scene_metadata.txt")

scene_metadata_file = open(path_scene_metadata, "w")
scene_metadata_file.write(scene_metadata)
scene_metadata_file.close()