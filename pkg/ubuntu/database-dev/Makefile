
BUILD_DIR=./hermod_0.2_database_dev

CP=cp -a
MAKE=make
MD=mkdir -p

all: prepare headers
	dpkg-deb --build $(BUILD_DIR)

prepare:
	@echo " * Create build directory $(BUILD_DIR)"
	@$(MD) $(BUILD_DIR)
	@cp -a DEBIAN $(BUILD_DIR)/DEBIAN
	@echo -n "Architecture: " >> $(BUILD_DIR)/DEBIAN/control
	@dpkg --print-architecture >> $(BUILD_DIR)/DEBIAN/control
	
headers: 
	@echo " * Install hermod header files"
	@$(MD) $(BUILD_DIR)/usr/include/hermod/Database
	@$(CP) ../../../modules/Database/*.hpp $(BUILD_DIR)/usr/include/hermod/Database
	
install:
	dpkg -i $(BUILD_DIR).deb

clean:
	rm -f $(BUILD_DIR).deb
	rm -rf $(BUILD_DIR)
	rm -f *~
