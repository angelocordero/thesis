import matplotlib.pyplot as plt
import numpy as np 

filename = "input/data.csv"

data = np.genfromtxt(filename, delimiter=",", names=None)  # No column names
lat = data[:, 1]
lon = data[:, 2]


plt.scatter(lon, lat)

plt.plot([lon[0], lon[-1]], [lat[0], lat[-1]], color="red", linestyle="--", linewidth=3)

for i in range(len(lon) - 1):
    plt.plot([lon[i], lon[i + 1]], [lat[i], lat[i + 1]], color="blue", linewidth=3, markersize = 50)

plt.xlabel("Longitude")
plt.ylabel("Latitude")
plt.title("Test 1")

plt.show()