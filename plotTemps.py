import matplotlib.pyplot as plt

# f = open('2022-04-27_16-01-27.log', 'r')
# f = open('2022-04-27_18-34-03.log', 'r')
# f = open('2022-04-27_21-11-48.log', 'r')
# f = open('2022-04-28_11-04-46.log', 'r')
# f = open('2022-04-28_11-05-18.log', 'r')
# f = open('2022-04-29_10-28-59.log', 'r')
# f = open('2022-04-29_10-38-07.log', 'r')
f = open('./temps_logs/2022-04-29_11-05-05.log', 'r')

temps = [float(line[:-1]) for line in f.readlines()]

plt.plot([i / 600.0 for i in range(len(temps))], temps)

plt.show()