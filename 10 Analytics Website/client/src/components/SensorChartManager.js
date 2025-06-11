import "./styles/SensorChartManager.css"
import "react-grid-layout-19/css/styles.css"
import React, { useState, useEffect, useMemo } from 'react';
import {WidthProvider, Responsive} from "react-grid-layout-19"
import SensorGraph from './SensorGraph'

const ResponsiveReactGridLayout = WidthProvider(Responsive);

export default function SensorChartManager({socket, isArrangementMode, onLayoutChange}) {

  const [clientChartSettings, setClientChartSettings] = useState(getDefaultClientChartSettings());

  const [serverChartSettings, setServerChartSettings] = useState(getDefaultServerChartSettings());

  const chartSettings = useMemo(() => {
    return clientChartSettings.map((clientSettings, i) => ({
      clientSettings: clientSettings,
      serverSettings: serverChartSettings[i]
    }))
  }, [clientChartSettings, serverChartSettings])

  const sensorCharts = chartSettings.map((settings) => {
      const clientSettings = settings.clientSettings;
      const serverSetttings = settings.serverSettings;
      return (
        <div key={serverSetttings.id} data-grid={{...clientSettings.layout, i: serverSetttings.id}}>
          <SensorGraph clientChartSettings={clientSettings} serverChartSettings={serverSetttings} isArrangementMode={false}></SensorGraph>
        </div>
      )
    }
  );

  return (
    <div className="SensorChartManager" style={{height: '100%'}}>
      <ResponsiveReactGridLayout
        className="layout"
        cols={{ lg: 12, md: 10, sm: 6, xs: 4, xxs: 2 }}
        rowHeight={30}
        isDraggable={isArrangementMode}
        isResizable={isArrangementMode}
        onLayoutChange={handleLayoutChange}
        compactType={null}
        preventCollision={true}
        useCSSTransforms={true}
        margin={[10, 10]}
        containerPadding={[0, 0]}
      >
        {sensorCharts}
      </ResponsiveReactGridLayout>
    </div>
  );

  function generateClientChartSettings() {
    return [
      {
        displayChart: true,
        layout: {
          w: 6,
          h: 8,
          x: 0,
          y: 0
        }
      },
      {
        displayChart: true,
        layout: {
          w: 6,
          h: 8,
          x: 6,
          y: 0
        }
      }
    ]
  }

  function getDefaultClientChartSettings() {
    return generateClientChartSettings();
  }

  function generateServerChartSettings() {
    return [
        {
        id: "0",
        datasets: [
          {
              label: "Brake Pressure Front",
              data: [],
              borderColor: "rgba(255, 99, 132, 1)", // Red
              fill: false,
              tension: 0.1
          },
          {
              label: "Brake Pressure Rear",
              data: [],
              borderColor: "rgb(179, 27, 141)", // Blue
              fill: false,
              tension: 0.1
          }
        ]
      },
      {
        id: "1",
        datasets: [
          {
            label: "Test Graph",
            data: [],
            borderColor: "rgba(41, 160, 59, 0)",
            fill: false,
            tension: 0.1
          }
        ]
      }
    ]
  }

  function getDefaultServerChartSettings() {
    return generateServerChartSettings();
  }

  function handleLayoutChange(layout) {
    // Update the client chart settings with new layout positions
    const updatedClientSettings = clientChartSettings.map((settings, index) => {
      const layoutItem = layout.find(item => item.i === serverChartSettings[index].id);
      if (layoutItem) {
        return {
          ...settings,
          layout: {
            w: layoutItem.w,
            h: layoutItem.h,
            x: layoutItem.x,
            y: layoutItem.y
          }
        };
      }
      return settings;
    });
    setClientChartSettings(updatedClientSettings);
    
    // Call parent's onLayoutChange if provided
    if (onLayoutChange) {
      onLayoutChange(layout);
    }
  }
}

/*
// Predefined chart configurations - hardcoded
const chartConfigs = {
  brakePressure: {
    title: 'Brake Pressure',
    yAxisLabel: 'PSI',
    datasets: [
      {
        label: 'Front Brake Pressure',
        dataKey: 'brakePresssureFront',
        color: 'rgba(255, 99, 132, 1)', // Red
      },
      {
        label: 'Rear Brake Pressure',
        dataKey: 'brakePressureRear',
        color: 'rgba(54, 162, 235, 1)', // Blue
      }
    ]
  },
  rpm: {
    title: 'RPM',
    yAxisLabel: 'RPM',
    datasets: [
      {
        label: 'Front Left Wheel',
        dataKey: 'RPMFrontLeft',
        color: 'rgba(75, 192, 192, 1)', // Teal
      },
      {
        label: 'Front Right Wheel',
        dataKey: 'RPMFrontRight',
        color: 'rgba(153, 102, 255, 1)', // Purple
      },
      {
        label: 'Rear Wheels',
        dataKey: 'RPMRear',
        color: 'rgba(255, 159, 64, 1)', // Orange
      }
    ]
  },
  speed: {
    title: 'Vehicle Speed',
    yAxisLabel: 'MPH',
    datasets: [
      {
        label: 'Speed',
        dataKey: 'speed',
        color: 'rgba(255, 206, 86, 1)', // Yellow
      }
    ]
  },
  temperature: {
    title: 'Temperature',
    yAxisLabel: '°F',
    datasets: [
      {
        label: 'CVT Temp',
        dataKey: 'CVTTemp',
        color: 'rgba(255, 99, 132, 1)', // Red
      },
      {
        label: 'Transfer Case Temp',
        dataKey: 'transferCaseTemp',
        color: 'rgba(54, 162, 235, 1)', // Blue
      },
      {
        label: 'Brake Temp',
        dataKey: 'brakeTemp',
        color: 'rgba(75, 192, 192, 1)', // Teal
      }
    ]
  },
  steering: {
    title: 'Steering Angle',
    yAxisLabel: 'Degrees',
    datasets: [
      {
        label: 'Steering Angle',
        dataKey: 'steeringAngle',
        color: 'rgba(153, 102, 255, 1)', // Purple
      }
    ]
  }
};
*/

/*
export default function SensorChartManager({ socket }) {
  // State for selected charts
  const [selectedCharts, setSelectedCharts] = useState({
    brakePressure: true,
    rpm: false,
    speed: false,
    temperature: false,
    steering: false
  });
  
  // State for the latest sensor data
  const [sensorData, setSensorData] = useState(null);
  
  // State for layout options
  const [layout, setLayout] = useState({
    columns: 1,
    chartWidth: '95%', 
    chartHeight: '400px'
  });

  // Listen for WebSocket messages and update sensorData
  useEffect(() => {
    if (!socket) return;

    // Function to handle incoming socket messages
    const handleSocketMessage = (event) => {
      try {
        const res = JSON.parse(event.data);
        if (res.msgType === "data") {
          setSensorData(res.content);
        }
      } catch (error) {
        console.error("Error processing socket message:", error);
      }
    };

    // Set up event listener
    socket.addEventListener("message", handleSocketMessage);

    // Request data from server
    if (socket.readyState === WebSocket.OPEN) {
      socket.send("data");
    } else {
      socket.addEventListener(
        "open",
        () => {
          socket.send("data");
        },
        { once: true }
      );
    }

    // Clean up event listener on unmount
    return () => {
      socket.removeEventListener("message", handleSocketMessage);
    };
  }, [socket]);

  // Toggle chart selection
  const toggleChart = (chartId) => {
    setSelectedCharts(prev => ({
      ...prev,
      [chartId]: !prev[chartId]
    }));
  };

  // Update layout settings
  const updateLayout = (columns) => {
    const width = columns === 1 ? '95%' : `${95/columns}%`;
    
    setLayout({
      columns,
      chartWidth: width,
      chartHeight: columns === 1 ? '400px' : '300px'
    });
  };

  // Get the active chart configs
  const activeCharts = Object.keys(selectedCharts)
    .filter(chartId => selectedCharts[chartId])
    .map(chartId => ({
      id: chartId,
      config: chartConfigs[chartId]
    }));
  
  return (
    <div className="sensor-chart-manager">
      <div className="controls">
        <div className="chart-selector">
          <h3>Select Charts</h3>
          <div className="chart-checkboxes">
            {Object.keys(chartConfigs).map(chartId => (
              <label key={chartId}>
                <input
                  type="checkbox"
                  checked={selectedCharts[chartId] || false}
                  onChange={() => toggleChart(chartId)}
                />
                {chartConfigs[chartId].title}
              </label>
            ))}
          </div>
        </div>
        
        <div className="layout-controls">
          <h3>Layout</h3>
          <div className="layout-buttons">
            <button onClick={() => updateLayout(1)}>Single Column</button>
            <button onClick={() => updateLayout(2)}>Two Columns</button>
          </div>
        </div>
      </div>
      
      <div 
        className="charts-container" 
        style={{ 
          display: 'flex', 
          flexWrap: 'wrap', 
          justifyContent: 'space-around'
        }}
      >
        {activeCharts.length === 0 ? (
          <div className="no-charts-message">
            <p>Select charts to display using the checkboxes above</p>
          </div>
        ) : (
          activeCharts.map(({ id, config }) => (
            <div 
              key={id} 
              style={{ 
                width: layout.chartWidth, 
                margin: '10px',
                boxSizing: 'border-box'
              }}
            >
              <SensorChart 
                chartId={id}
                sensorConfig={config}
                sensorData={sensorData}
                width="100%"
                height={layout.chartHeight}
              />
            </div>
          ))
        )}
      </div>
    </div>
  );
} */