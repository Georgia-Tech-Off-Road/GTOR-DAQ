def convertBrakePressure(analogValue):
    return (5 * (analogValue/1023) - 0.5) * 500
