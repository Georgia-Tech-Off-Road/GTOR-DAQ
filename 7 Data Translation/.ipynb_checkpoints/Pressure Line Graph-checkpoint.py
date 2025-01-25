
# Creates a line graph of brake pressure vs. time

import pandas as pd
import plotly.express as px

# Create df from a tetx file
file = 'output.txt'
df = pd.read_csv(file, delimiter=',')

# Get value from DF
val = df.iloc[:, 5] 
df['Index'] = range(len(val))

# Create line graph
fig = px.line(df, x='Index', y=val, title='Brake Pressure', labels={'Index':'Microseconds', 'y':'Pressure', 5: 'Column 6'})
fig.show() # Displays graph
print("Done")
