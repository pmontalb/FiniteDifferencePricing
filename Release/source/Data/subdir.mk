################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/Data/CDividend.cpp \
../source/Data/CInputData.cpp \
../source/Data/COutputData.cpp 

OBJS += \
./source/Data/CDividend.o \
./source/Data/CInputData.o \
./source/Data/COutputData.o 

CPP_DEPS += \
./source/Data/CDividend.d \
./source/Data/CInputData.d \
./source/Data/COutputData.d 


# Each subdirectory must supply rules for building sources it contributes
source/Data/%.o: ../source/Data/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++1y -I"/home/raiden/workspace/FiniteDifferencePricing/include" -I/usr/include/gtest -O3 -pedantic -pedantic-errors -Wall -Wextra -Werror -c -pipe -fmessage-length=0 -fstrict-aliasing  -Wfatal-errors -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


