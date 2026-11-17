include $(TOPDIR)/rules.mk

PKG_NAME:=usb-test
PKG_VERSION:=1.0.2
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/usb-test
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=USB updater test package
  PKGARCH:=all
endef

define Package/usb-test/description
 A harmless marker package used to verify TESPRO USB APK auto-install.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/files
	cp ./files/README.txt ./files/installed.txt ./files/usb-test ./files/usb-test.init ./files/usb-test.cgi $(PKG_BUILD_DIR)/files/
endef

define Build/Compile
endef

define Package/usb-test/install
	$(INSTALL_DIR) $(1)/usr/share/usb-test
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/README.txt $(1)/usr/share/usb-test/README.txt
	$(INSTALL_DIR) $(1)/etc/usb-test
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/installed.txt $(1)/etc/usb-test/installed.txt
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/www/cgi-bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/usb-test $(1)/usr/sbin/usb-test
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/usb-test.init $(1)/etc/init.d/usb-test
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/usb-test.cgi $(1)/www/cgi-bin/usb-test
endef

define Package/usb-test/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || { /etc/init.d/usb-test enable; /etc/init.d/usb-test restart; }
exit 0
endef

$(eval $(call BuildPackage,usb-test))
