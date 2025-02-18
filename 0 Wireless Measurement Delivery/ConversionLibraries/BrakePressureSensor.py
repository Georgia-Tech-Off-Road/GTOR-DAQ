#function to convert analog values to brake pressure readings
def convertBrakePressure(analogValue):
    return (5 * (analogValue/1023) - 0.5) * 500
