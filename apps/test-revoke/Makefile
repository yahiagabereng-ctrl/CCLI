include $(TOPDIR)/rules.mk

PKG_NAME:=test-revoke
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/test-revoke
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Unsigned startup and signature-revocation test package
  PKGARCH:=all
endef

define Package/test-revoke/description
  Intentionally unsigned test package. When manually installed, it creates
  an enabled init service so it is visible in LuCI Startup.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/files
	cp ./files/test-revoke $(PKG_BUILD_DIR)/files/
	cp ./files/test-revoke.init $(PKG_BUILD_DIR)/files/
	cp ./files/test-revoke.cgi $(PKG_BUILD_DIR)/files/
	cp ./files/README.txt $(PKG_BUILD_DIR)/files/
	cp ./files/installed.txt $(PKG_BUILD_DIR)/files/
endef

define Build/Compile
endef

define Package/test-revoke/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/test-revoke $(1)/usr/share/test-revoke $(1)/www/cgi-bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/test-revoke $(1)/usr/sbin/test-revoke
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/test-revoke.init $(1)/etc/init.d/test-revoke
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/test-revoke.cgi $(1)/www/cgi-bin/test-revoke
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/README.txt $(1)/usr/share/test-revoke/README.txt
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/installed.txt $(1)/etc/test-revoke/installed.txt
endef

define Package/test-revoke/postinst
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ]; then
    /etc/init.d/test-revoke enable
    /etc/init.d/test-revoke start
fi
exit 0
endef

$(eval $(call BuildPackage,test-revoke))
