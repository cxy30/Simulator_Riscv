import matplotlib.pyplot as plt

file = open("block_size.txt", "r")
i = 0
j = 0
x = []
y = []
for line in file:
	l = line.strip().split("\t")
	x.append(l[1])
	y.append(l[2])
	j = j + 1
	if (j == 8):
		plt.plot(x, y)
		x = []
		y = []
		j = 0
plt.show()