################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Game/Block.cpp \
../Game/Board.cpp \
../Game/Game.cpp \
../Game/GarbageBlock.cpp 

OBJS += \
./Game/Block.o \
./Game/Board.o \
./Game/Game.o \
./Game/GarbageBlock.o 

CPP_DEPS += \
./Game/Block.d \
./Game/Board.d \
./Game/Game.d \
./Game/GarbageBlock.d 


# Each subdirectory must supply rules for building sources it contributes
Game/%.o: ../Game/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


