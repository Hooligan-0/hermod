
BUILD_DIR=./hermod_0.2

CP=cp -a
MAKE=make
MD=mkdir -p

all: prepare core modules
	dpkg-deb --build $(BUILD_DIR)

prepare:
	@echo " * Create build directory $(BUILD_DIR)"
	@$(MD) $(BUILD_DIR)
	@cp -a DEBIAN $(BUILD_DIR)/DEBIAN
	@echo -n "Architecture: " >> $(BUILD_DIR)/DEBIAN/control
	@dpkg --print-architecture >> $(BUILD_DIR)/DEBIAN/control
	
core: ../../../src/hermod
	@echo " * Install hermod program"
	@$(MD) $(BUILD_DIR)/usr/sbin
	@$(CP) ../../../src/hermod $(BUILD_DIR)/usr/sbin
	@echo " * Insert config file"
	@$(MD) $(BUILD_DIR)/etc/hermod
	@$(CP) config/main.cfg $(BUILD_DIR)/etc/hermod
	
modules: ../../../modules/Dummy/dummy.so
	@echo " * Insert Dummy module"
	@$(MD) $(BUILD_DIR)/usr/lib/hermod
	@$(CP) ../../../modules/Dummy/dummy.so $(BUILD_DIR)/usr/lib/hermod

install:
	dpkg -i $(BUILD_DIR).deb

clean:
	rm -f $(BUILD_DIR).deb
	rm -rf $(BUILD_DIR)
	rm -f *~

../../../src/hermod:
	@$(MAKE) -C ../../../src/
../../../modules/Dummy/dummy.so:
	@$(MAKE) -C ../../../modules/Dummy
