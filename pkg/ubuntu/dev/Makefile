
BUILD_DIR=./hermod_0.3.1_dev

CP=cp -a
MAKE=make
MD=mkdir -p

all: prepare headers modules
	dpkg-deb --build $(BUILD_DIR)

prepare:
	@echo " * Create build directory $(BUILD_DIR)"
	@$(MD) $(BUILD_DIR)
	@cp -a DEBIAN $(BUILD_DIR)/DEBIAN
	@echo -n "Architecture: " >> $(BUILD_DIR)/DEBIAN/control
	@dpkg --print-architecture >> $(BUILD_DIR)/DEBIAN/control
	
headers: 
	@echo " * Install hermod hreder files"
	@$(MD) $(BUILD_DIR)/usr/include/hermod
	@$(MD) $(BUILD_DIR)/usr/include/hermod/ContentHtml
	@$(MD) $(BUILD_DIR)/usr/include/hermod/ContentJson
	@$(CP) ../../../src/*.hpp $(BUILD_DIR)/usr/include/hermod
	@$(CP) ../../../src/ContentHtml/*.hpp $(BUILD_DIR)/usr/include/hermod/ContentHtml
	@$(CP) ../../../src/ContentJson/*.hpp $(BUILD_DIR)/usr/include/hermod/ContentJson
	
modules: ../../../modules/Dummy/dummy.so
	@echo " * Insert Dummy module headers"
	@$(MD) $(BUILD_DIR)/usr/include/hermod/Dummy
	@$(CP) ../../../modules/Dummy/*.hpp $(BUILD_DIR)/usr/include/hermod/Dummy

install:
	dpkg -i $(BUILD_DIR).deb

clean:
	@echo " * Remove package file and build dir $(BUILD_DIR)"
	@rm -f $(BUILD_DIR).deb
	@rm -rf $(BUILD_DIR)
	@echo " * Remove temporary files ( *~ )"
	@rm -f *~ config/*~ DEBIAN/*~
