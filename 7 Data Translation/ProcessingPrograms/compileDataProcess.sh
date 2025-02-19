#Colors
ERROR_COLOR='\033[0;32m'
WARNING_COLOR='\033[1;33m'
GOOD_COLOR='\033[32m'
DEFAULT_COLOR='\033[0m'
#
#Output file name
OUTPUT_EXECUTABLE_NAME="dataprocess"
#
#Delete old executable
CMD="rm ${OUTPUT_EXECUTABLE_NAME}"
eval "$CMD"
echo -e "${GOOD_COLOR}Deleted old executable file!"
echo -e "${DEFAULT_COLOR}"
#
#Define dependent .cpp files, DO NOT INCLUDE .cpp in filepath. INCLUDE EVERYTHING BUT .cpp
DEPENDENCIES=()
#
#Add all your necessary .cpp files down here. DO NOT INCLUDE .cpp in filepath. INCLUDE EVERYTHING BUT .cpp in the file path!!!
DEPENDENCIES+=("DAQSensor")
#
DEPENDENCIES_CPP=()
#
#Define colors for echo output
ERROR_COLOR='\033[0;32m'
WARNING_COLOR='\033[1;33m'
#Add .cpp to end of files
for i in "${!DEPENDENCIES[@]}"; do
  DEPENDENCIES_CPP[$i]="${DEPENDENCIES[$i]}.cpp"
done
#
#Run linter on all .cpp files
for i in "${!DEPENDENCIES_CPP[@]}"; do
  CMD="cpplint ${DEPENDENCIES_CPP[i]}"
  echo -e "Executing ${i}: ${CMD}\n"
  echo -e "${WARNING_COLOR}$(eval "$CMD")"
done
eval "cpplint DataTranslator.cpp"
#
#Define compilation settings
COMPILE_SETTINGS="-std=c++20 -Wall -Wextra -g -pedantic -Wno-variadic-macros"
#
CMD="g++ ${COMPILE_SETTINGS} ${DEPENDENCIES_CPP[@]} DataTranslator.cpp -o ${OUTPUT_EXECUTABLE_NAME}"
echo -e "${GOOD_COLOR}Executing: ${CMD}${DEFAULT_COLOR}\n"
eval "$CMD"