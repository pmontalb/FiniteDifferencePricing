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
	g++-7 -std=c++1y -I"/home/raiden/workspace/FiniteDifferencePricing/include" -I/usr/include/gtest -O3 -m64 -march=native -flto=8 -floop-interchange -ftree-loop-distribution -floop-strip-mine -floop-block -ftree-vectorize -pedantic -pedantic-errors -Wall -Wextra -Werror -c -pipe -fmessage-length=0 -fstrict-aliasing  -Wfatal-errors -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


