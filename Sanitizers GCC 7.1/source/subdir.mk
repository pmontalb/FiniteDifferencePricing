################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/FiniteDifferencePricing.cpp 

OBJS += \
./source/FiniteDifferencePricing.o 

CPP_DEPS += \
./source/FiniteDifferencePricing.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++-7 -std=c++1y -I"/home/raiden/workspace/FiniteDifferencePricing/include" -I/usr/include/gtest -O0 -g3 -p -pg -ftest-coverage -fprofile-arcs -pedantic -pedantic-errors -Wall -Wextra -Werror -c -fmessage-length=0 -fstrict-aliasing  -Wfatal-errors -v -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


