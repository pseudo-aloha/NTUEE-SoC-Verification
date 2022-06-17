import numpy as np
import random
n = 100000
with open("input.pattern", "w") as f:
  f.write(str(n) + " " + str(11) + "\n")
  f.write("XXXXXXXXXX0\n")
  for i in range(n):
    arr = random.choice(["01", "10", "11"])
    f.write(arr)
    arr = np.random.randint(2, size=(8,))
    for j in range(arr.size):
      f.write(str(arr[j]))
    f.write("1\n")