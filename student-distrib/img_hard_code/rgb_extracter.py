#  @fileoverview rgb_extracter.py - Extract RGB color from a jpg image 
#                                     and convert to RGB565 format,
#                                     then store it inside a .h file
#  @author Kunle Li
#  @reference: https://docs.microsoft.com/en-us/windows/win32/directshow/working-with-16-bit-rgb
#  @date   May 3rd, 2022    First done
#          May 4th, 2022    Add args to automatically change the file name
#                           and the variable name


from PIL import Image
import numpy as np
import argparse
import os

parser = argparse.ArgumentParser(description='image file name')
parser.add_argument('img_name', action='store', type=str, help='image name')

args = parser.parse_args()

assert(os.path.exists('./{}.jpg'.format(args.img_name)))

img = Image.open('./{}.jpg'.format(args.img_name)).convert('RGB') 
img = np.array(img)

x = np.linspace(0, img.shape[1] - 1, img.shape[1])
y = np.linspace(0, img.shape[0] - 1, img.shape[0])
xv, yv = np.meshgrid(x, y) 
rgb = img[np.int32(yv), np.int32(xv), :].astype(np.int32)
data = ((rgb[:, :, 0] >> 3) << 11) + ((rgb[:, :, 1] >> 2) << 5) + (rgb[:, :, 2] >> 3) # RGB 565

with open('{}_color.h'.format(args.img_name), 'w') as f:
    f.write('#ifndef _{}_COLOR_H\n'.format(args.img_name.upper()))
    f.write('#define _{}_COLOR_H\n\n'.format(args.img_name.upper()))
    f.write('#include "types.h"\n#include "vbe.h"\n\n')
    f.write('// Resolution: {} * {}\n\n'.format(img.shape[0], img.shape[1]))
    f.write('const int16_t {}_bg[{}_BG_WIDTH * {}_BG_HEIGHT]'.format(args.img_name, args.img_name.upper(), args.img_name.upper()))
    f.write(' = {\n')
    for i in range(data.shape[0]):
        for j in range(data.shape[1]):
            if i == data.shape[0] - 1 and j == data.shape[1] - 1:
                f.write('%s' % hex(data[i][j]))
            else:
                f.write('%s,' % hex(data[i][j]))
            if(24 == j % 30):
                f.write('\n')
    f.write('};\n\n')
    f.write('#endif\n')            
                

                