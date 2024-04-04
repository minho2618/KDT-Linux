obj-m := k_spi.o

export BUILDROOT=/home/kesl/grepp/src/buildroot
export ARCH=arm64
export CROSS_COMPILE=$(BUILDROOT)/output/host/bin/aarch64-buildroot-linux-gnu-
export DTC=/home/kesl/grepp/src/buildroot/output/host/bin/dtc

KERNELDIR ?= $(BUILDROOT)/output/build/linux-custom

all default: modules
install: modules_install

modules modules_install help clean:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) $@

dt: disable_spidev.dts
	$(DTC) -@ -I dts -O dtb -o disable_spidev.dtbo disable_spidev.dts

.PHONY: nfs
nfs:
	cp k_spi.ko ~/nfs
	cp disable_spidev.dtbo ~/nfs
