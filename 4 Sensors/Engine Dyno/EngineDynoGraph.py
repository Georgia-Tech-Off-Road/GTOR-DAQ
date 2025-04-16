import plotly.express as px
import pandas as pd

def dynoGraph(filePath):
    data = pd.read_csv(filePath, skiprows=10, header=None, engine='python')
    time = data.iloc[:, 1] / (10 ** 6)

    fig = px.line(data, x=data.columns[1], y=data.columns[3], title="Engine Dyno Graph")

    fig.update_layout(
        xaxis_title="RPM",  # Label for the x-axis
        yaxis_title="Pounds",  # Label for the y-axis
        legend_title="Engine Dyno",  # Legend title
        legend=dict(title="Legend", orientation="h", yanchor="bottom", y=1.02, xanchor="right", x=1)
    )

    # Show the plot
    fig.show()

# Call the function with the correct file path
dynoGraph('2025-04-10_19-06-26.csv')
