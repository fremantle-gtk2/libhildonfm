SUBDIRS = hildon-fm doc tests

EXTRA_DIST =					\
	debian/changelog			\
	debian/control				\
	debian/copyright			\
	debian/rules				\
	debian/libhildonfm2.install		\
	debian/libhildonfm2-dbg.install		\
	debian/libhildonfm2-dev.install		\
	debian/libhildonfm2-dev.docs		\
	debian/hildon-fm1.links			\
	debian/hildon-fm1.install		\
	debian/hildon-fm-dev.links		\
	debian/hildon-fm-dev.install


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = hildon-fm-2.pc hildon-fm.pc

xdgdir = /etc/xdg
# FIXME: Use $(sysconfdir)/xdg if the prefix is not /usr
xdg_DATA = data/user-dirs.defaults

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure config.h.in compile \
		       ltmain.sh depcomp missing install-sh config.sub \
		       config.guess

deb: dist
	-mkdir debian-build
	cd debian-build &&						\
	rm -rf $(PACKAGE)-$(VERSION) &&					\
	tar zxvf ../$(PACKAGE)-$(VERSION).tar.gz &&			\
	cd $(PACKAGE)-$(VERSION) && dpkg-buildpackage -rfakeroot
