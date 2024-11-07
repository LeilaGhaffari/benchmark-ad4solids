#!/usr/bin/env python3

import numpy as np
import pandas as pd
import sys

if len(sys.argv) != 2:
    print("Usage: python generate_data.py <number of quadrature points>")
    sys.exit(1)

try:
    Q = int(sys.argv[1])
except ValueError:
    print("Error: Please provide a valid integer for the number of quadrature points.")
    sys.exit(1)

np.random.seed(42)

data = {
    'dXdx_init': np.random.random(Q * 9),
    'dudX': np.random.random(Q * 9),
    'ddudX': np.random.random(Q * 9),
}
df = pd.DataFrame(data)

df.to_csv('random-data.csv', index=False)

print(f"Data saved to 'random-data.csv' with {Q} quadrature points.")
