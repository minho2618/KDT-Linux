obj-m := kdt_device_tree.o

export BUILDROOT=/home/kesl/grepp/src/buildroot
export ARCH=arm64
export CROSS_COMPILE=$(BUILDROOT)/output/host/bin/aarch64-buildroot-linux-gnu-
export DTC=/home/kesl/grepp/src/buildroot/output/host/bin/dtc

KERNELDIR ?= $(BUILDROOT)/output/build/linux-custom

all default: modules
install: modules_install

modules modules_install help clean:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) $@

dt: kdt.dts
	$(DTC) -@ -I dts -O dtb -o kdt.dtbo kdt.dts

.PHONY: nfs
nfs:
	cp kdt_device_tree.ko ~/nfs
	cp kdt.dtbo ~/nfs

