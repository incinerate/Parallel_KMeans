
# encoding=utf-8
import matplotlib.pyplot as plt
from pylab import *                                 #支持中文
mpl.rcParams['font.sans-serif'] = ['SimHei']

names = ['2', '4', '6', '8', '12', '16', '32', '64', '128', '256', '512']
x = range(len(names))
x1 = range(len(names)-1)
y = [0.0034,0.0148,0.0309,0.0343,0.0520,0.0808,0.286,0.710,1.358,2.157,3.841]
y1=[0.0045,0.0244,0.0356,0.0335,0.0715,0.0836,0.3156,0.6457,2.562,6.477,9.489]
#plt.plot(x, y, 'ro-')
#plt.plot(x, y1, 'bo-')
#pl.xlim(-1, 11)  # 限定横轴的范围
# plt.ylim(0, 6.2)  # 限定纵轴的范围
# plt.subplot(121)
plt.plot(x, y, color='red', mec='r', mfc='w',label=u'MPI')
plt.plot(x, y1, ms=10, label=u'Sequential')
for a, b in zip(x, y):
    plt.text(a, b, b, ha='center', va='top', fontsize=10)
plt.legend()  # 让图例生效
plt.margins(0)
plt.xticks(x, names, rotation=45)
plt.subplots_adjust(bottom=0.15)
plt.ylabel("Time(s)") #Y轴标签
plt.xlabel(u"dimension(2 clusters, 15000 points)") #X轴标签
plt.title("Seq vs MPI(different dimensions)") #标题
    
    
# plt.subplot(122)

    
for a, b in zip(x, y1):
    plt.text(a, b, b, ha='center', va='bottom', fontsize=10)


plt.show()
