#Define dependent .cpp files, DO NOT INCLUDE .cpp in filepath. INCLUDE EVERYTHING BUT .cpp
DEPENDENCIES=()

#Add all your necessary .cpp files down here. DO NOT INCLUDE .cpp in filepath. INCLUDE EVERYTHING BUT .cpp in the file path!!!
DEPENDENCIES+=("../ConversionLibraries/BrakePressure/BrakePressureConvert")
DEPENDENCIES+=("DAQSensors")
DEPENDENCIES+=("../ConversionLibraries/Time/TimeConvert")

DEPENDENCIES_CPP=()
DEPENDENCIES_O=()
DEPENDENCIES_H=()

#Define colors for echo output
ERROR_COLOR='\033[0;32m'
WARNING_COLOR='\033[1;33m'
#Add .cpp or .o to end of files
for i in "${!DEPENDENCIES[@]}"; do
  DEPENDENCIES_CPP[$i]="${DEPENDENCIES[$i]}.cpp"
  DEPENDENCIES_O[$i]="${DEPENDENCIES[$i]}.o"
  DEPENDENCIES_H[$i]="${DEPENDENCIES[$i]}.h"
done


#Compile library .cpp files to .o object files
for i in "${!DEPENDENCIES[@]}"; do
  CMD="g++ -c ${DEPENDENCIES_CPP[$i]} -o ${DEPENDENCIES_O[$i]}"
  eval "$CMD"
done

#Run linter on all .cpp files
for i in "${!DEPENDENCIES_CPP[@]}"; do
  CMD="cpplint ${DEPENDENCIES_CPP[i]}"
  echo -e "Executing ${i}: ${CMD}\n"
  echo -e "${WARNING_COLOR}$(eval "$CMD")"
done
eval "cpplint DataTranslator.cpp"

#Define compilation settings
COMPILE_SETTINGS="-std=c++20 -Wall -Wextra -pedantic"

#Compile DataTranslator.cpp
CMD="g++ ${COMPILE_SETTINGS} -c DataTranslator.cpp -o DataTranslator.o $INCLUDE_ARG"
eval "$CMD"

CMD="g++ ${COMPILE_SETTINGS} ${DEPENDENCIES_O[@]} DataTranslator.o -o dataprocess"
eval "$CMD"