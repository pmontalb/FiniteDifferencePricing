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
	/opt/intel/bin/icc -std=c++1y -I"/home/raiden/workspace/FiniteDifferencePricing/include" -I/usr/include/gtest -O3 -no-prec-div -no-prec-sqrt -ansi-alias -xHost -ipo -fp-model fast=2 -fimf-precision=low -vec-threshold=80 -qopt-report3 -fno-alias -pedantic -pedantic-errors -Wall -Wextra -Werror -c -pipe -fmessage-length=0 -fstrict-aliasing  -Wfatal-errors -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


