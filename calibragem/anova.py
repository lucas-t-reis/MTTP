import pandas as pd
import matplotlib.pyplot as plt

datafile = "anova.csv"
data = pd.read_csv(datafile)

#Create a boxplot
data.boxplot('result', by='method', figsize=(12, 8))
plt.savefig("anova.png")
