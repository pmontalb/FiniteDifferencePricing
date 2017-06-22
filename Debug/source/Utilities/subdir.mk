################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/Utilities/CPlotter.cpp \
../source/Utilities/CStats.cpp 

OBJS += \
./source/Utilities/CPlotter.o \
./source/Utilities/CStats.o 

CPP_DEPS += \
./source/Utilities/CPlotter.d \
./source/Utilities/CStats.d 


# Each subdirectory must supply rules for building sources it contributes
source/Utilities/%.o: ../source/Utilities/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++1y -I"/home/raiden/workspace/FiniteDifferencePricing/include" -I/usr/include/gtest -O0 -g3 -pedantic -pedantic-errors -Wall -Wextra -Werror -c -pipe -fmessage-length=0 -fstrict-aliasing  -Wfatal-errors -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


