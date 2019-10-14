#XX = g++
PROJECT_DIR = $(shell pwd)
PROM    = ota

export ARMCXX = $(HOST_DIR)/bin/arm-rockchip-linux-gnueabihf-g++

CXXFLAGS ?= -Wall -fPIC -O3 -ldl -D_USE_LINUX 

all:ota

ota:main.cpp
	$(ARMCXX) $(CXXFLAGS) main.cpp -L./libs/ -lcurl -lnghttp2 -lOtaUpgrade -lpthread -o ota
	@echo "Build successfully"
# -lpthread -lrt -lcurl -ljson 

.PHONY : install clean  
clean:
	rm -rf ota
	@echo "clean done" 
install:
	sudo install -D -m 755 ota -t /usr/bin/
	#cp -f ota $(TARGET_DIR)/usr/bin/
	sudo cp -rf ./libs/*.* /usr/lib/
