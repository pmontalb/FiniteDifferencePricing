################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/BlackScholes/CBlackScholes.cpp 

OBJS += \
./source/BlackScholes/CBlackScholes.o 

CPP_DEPS += \
./source/BlackScholes/CBlackScholes.d 


# Each subdirectory must supply rules for building sources it contributes
source/BlackScholes/%.o: ../source/BlackScholes/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++-7 -std=c++1y -I"/home/raiden/workspace/FiniteDifferencePricing/include" -I/usr/include/gtest -O3 -pedantic -pedantic-errors -Wall -Wextra -Werror -c -pipe -fmessage-length=0 -fstrict-aliasing  -Wfatal-errors -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


