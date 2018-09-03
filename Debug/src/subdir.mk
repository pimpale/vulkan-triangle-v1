################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/error_methods.c \
../src/glfw_methods.c \
../src/main.c \
../src/memory.c \
../src/vulkan_methods.c 

OBJS += \
./src/error_methods.o \
./src/glfw_methods.o \
./src/main.o \
./src/memory.o \
./src/vulkan_methods.o 

C_DEPS += \
./src/error_methods.d \
./src/glfw_methods.d \
./src/main.d \
./src/memory.d \
./src/vulkan_methods.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c11 -I/usr/include/vulkan -I/usr/include/GLFW -O0 -g3 -pedantic -Wall -Wextra -Wconversion -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


