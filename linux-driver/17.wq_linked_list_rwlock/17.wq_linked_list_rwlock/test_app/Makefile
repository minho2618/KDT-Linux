BUILDROOT_DIR=/home/kesl/grepp/src/buildroot/
TOOLCHAIN_PATH=${BUILDROOT_DIR}/output/host/bin
CC=${TOOLCHAIN_PATH}/aarch64-buildroot-linux-gnu-gcc
CXX=${TOOLCHAIN_PATH}/aarch64-buildroot-linux-gnu-g++

TARGET = wq_test

objects = main.o

$(TARGET): $(objects) 
	$(CXX) -o $(TARGET) $(objects)

main.o: main.c
	$(CC) -g $(INCLUDES) -c main.c

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf $(TARGET)


