################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/TestBlackScholes.cpp \
../tests/TestEvolutionOperator.cpp \
../tests/TestFDPricer.cpp \
../tests/TestGrid.cpp 

OBJS += \
./tests/TestBlackScholes.o \
./tests/TestEvolutionOperator.o \
./tests/TestFDPricer.o \
./tests/TestGrid.o 

CPP_DEPS += \
./tests/TestBlackScholes.d \
./tests/TestEvolutionOperator.d \
./tests/TestFDPricer.d \
./tests/TestGrid.d 


# Each subdirectory must supply rules for building sources it contributes
tests/%.o: ../tests/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	clang++-4.0 -std=c++1y -I"/home/raiden/workspace/FiniteDifferencePricing/include" -I/usr/include/gtest -O0 -g3 -p -pg -ftest-coverage -fprofile-arcs -pedantic -pedantic-errors -Wall -Wextra -Werror -c -pipe -fmessage-length=0 -fstrict-aliasing  -Wfatal-errors -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


