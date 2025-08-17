import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
df = pd.read_csv("signals_with_est.csv")

# Choose two columns to compare (edit as needed)
x_col = "SOC_Estimate"       # x-axis
y_col = "SOC"           # y-axis

plt.figure(figsize=(10,6))
plt.plot(df[x_col], label=x_col)
plt.plot(df[y_col], label=y_col)

plt.title(f"Comparison of {x_col} and {y_col}")
plt.xlabel("Index (Time step)")
plt.ylabel("Value")
plt.legend()
plt.grid(True)
plt.show()
