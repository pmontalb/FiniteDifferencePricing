################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/FiniteDifference/CGrid.cpp 

OBJS += \
./source/FiniteDifference/CGrid.o 

CPP_DEPS += \
./source/FiniteDifference/CGrid.d 


# Each subdirectory must supply rules for building sources it contributes
source/FiniteDifference/%.o: ../source/FiniteDifference/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++-7 -std=c++1y -I"/home/raiden/workspace/FiniteDifferencePricing/include" -I/usr/include/gtest -O0 -g3 -p -pg -ftest-coverage -fprofile-arcs -pedantic -pedantic-errors -Wall -Wextra -Werror -c -pipe -fmessage-length=0 -fstrict-aliasing  -Wfatal-errors -v -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


