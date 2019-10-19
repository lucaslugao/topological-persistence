import matplotlib.pyplot as plt
import numpy as np

def f(x):
    return -1.152883833e10-17* x**3 + 4.066416867e10-11*x**2 - 1.258470521e10-6*x + 7.079834375e10-1
Ps = [['A',428643,6.732],['B',108161,1.033],['C',180347,1.736],['D',2716431,60+6.260]]
fig, ax = plt.subplots()

X = np.linspace(min([x[1] for x in Ps]), max([x[1] for x in Ps]), 50)

plt.plot(X,[f(x) for x in X], 'b')
for p in Ps:
    plt.plot([p[1]],[p[2]], 'or')
    ax.text(p[1]+20000,p[2]+1,p[0], fontsize=10)
plt.xlabel('m')
plt.ylabel('time (s)')
plt.title('Run times for the given filtrations')
plt.show()
