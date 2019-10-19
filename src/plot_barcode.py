import matplotlib.pyplot as plt
import numpy as np
import sys

if(len(sys.argv) != 2):
    print('Syntax: {0} <barcode_file>'.format(sys.argv[0]))
    exit()

inf = float('inf')
with open(sys.argv[1]) as f:
    barcode = [b.split() for b in f]
    barcode = [[int(x[0]), float(x[1]), float(x[2])] for x in barcode]
    dims = [x[0] for x in barcode]
    dims = [[x,dims.count(x)] for x in set(dims)]
    counter = [0 for _ in dims]
    heights = []
    for b in barcode:
        A = -b[0]
        B = -b[0] - 1
        v = counter[b[0]]
        n = dims[b[0]][1]

        t = (n - v)/(n+1)
        heights.append(t * A + (1-t) * B)

        #heights.append(-b[0] - (counter[b[0]]+0.5)/)
        counter[b[0]] += 1
      

    max_val = max([x[2] for x in barcode if x[2] != inf]) * 1.2
    min_val = min([x[1] for x in barcode if x[2] != inf])
    data = []
    if True:
        for h, bar in zip(heights, barcode):
            plt.plot([max_val if bar[2] == inf else bar[2], bar[1]], [h, h], '->b' if  bar[2] == inf  else 'b' ,markevery=2)
        
        for dim in dims[1:]:
            plt.axhline(y=-dim[0], color='gray', linestyle='-', linewidth=1)

        plt.yticks([-d[0]-1/2 for d in dims], ['$H_{}$'.format(d[0]) for d in dims])
        axes = plt.gca()
        axes.set_ylim([-1-max([x[0] for x in dims]), 0])
        plt.title(sys.argv[1].split('.')[-2].split('/')[-1])
        plt.show()
    else:
        scale = [12.0 / max_val, 3.0]
        for h, bar in zip(heights, barcode):
            print('\draw[{}] ({},{}) -- ({},{});'.format('-{Latex[scale=1.5]}' if bar[2] == inf else '[-)',
                bar[1]*scale[0], h*scale[1], max_val*scale[0] if bar[2] == inf else bar[2]*scale[0], h*scale[1]))
        for d in dims:
            print('\draw[rounded corners=0.2cm, dashed, color=gray] ({},{}) rectangle ++({},{});'.format((-0.1) * scale[0],-d[0]*scale[1],
             (max_val + 0.2)*scale[0], -1*scale[1]))
            print("\\node[text width=1cm, anchor=east, align=right] at ({},{}) {{$H_{}$}};".format((-0.1) * scale[0], (-0.5-d[0])*scale[1], d[0]))
        axis_h = (-1.2-dims[-1][0])*scale[1]
        print('\draw[-{{Latex[scale=1.5]}}] ({}, {}) -- ({}, {});'.format((-0.1) * scale[0],axis_h,
             (max_val + 0.1)*scale[0], axis_h))
        subdiv = max_val/10
        n = 0
        while(int(subdiv*10**n) == 0):
            n += 1
        subdiv = int(subdiv*10**n)/10**n

        for i in range(int(max_val/subdiv) + 1):
            print('\draw[-] ({}, {}) -- ({}, {});'.format((min_val -i * subdiv) * scale[0], axis_h-0.025*scale[1],(min_val -i * subdiv) * scale[0], axis_h+0.025*scale[1] ))
            print("\\node[text width=1cm, anchor=north, align=center] at ({},{}) {{${:.2f}$}};".format((min_val -i * subdiv) * scale[0], axis_h-0.05*scale[1] , i*subdiv))
      