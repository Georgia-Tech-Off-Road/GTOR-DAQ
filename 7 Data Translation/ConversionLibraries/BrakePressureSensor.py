#function to convert analog values to brake pressure readings
def convertBrakePressure(analogValue):
    return 50+(((analogValue/32767)*4.096-.5)/4)*7950
