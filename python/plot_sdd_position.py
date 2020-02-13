import matplotlib.pyplot as plt
import protocol as prt

f = open("./data/data5.pack", "r")

data = f.readline()
dx = []
dy = []
i = 0
while data != '':
        i = i + 1
        if(i == 150):
                break
        ox = int(data.split(" ")[0])
        oy = int(data.split(" ")[1][:-1])
        dx.append(ox)
        dy.append(oy)
        data = f.readline()

                

#plt.plot([i for i in range(len(dx))], dx)
#plt.plot([i for i in range(len(dy))], dy)
plt.plot(dx, dy)

plt.show()
	
