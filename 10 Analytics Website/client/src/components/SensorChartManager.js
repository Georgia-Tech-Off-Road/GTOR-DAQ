import "./styles/SensorChartManager.css"
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
        <div key={serverSetttings.id} data-grid={{
          ...clientSettings.layout, 
          i: serverSetttings.id,
          resizeHandles: ['s', 'w', 'n', 'e', 'ne', 'se', 'sw', 'nw']}}>
          <SensorGraph clientChartSettings={clientSettings} serverChartSettings={serverSetttings} isArrangementMode={isArrangementMode}></SensorGraph>
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
        compactType={'vertical'}
        preventCollision={false}
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