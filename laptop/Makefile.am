INCLUDES = $(all_includes) 
LDADD = $(all_libraries) -lkdeui -lkfile -lkfm -lmediatool -lkdecore -lqt -lXext -lX11 $(LIBSOCKET) $(KDE_RPATH)

bin_PROGRAMS = kcmlaptop

SUBDIRS = applnk doc

kcmlaptop_SOURCES = main.cpp power.cpp pcmcia.cpp warning.cpp battery.cpp kbattery.cpp daemon.cpp notify.cpp apmlib.c 
BUILT_SOURCES = power.moc pcmcia.moc warning.moc battery.moc kbattery.moc daemon.moc notify.moc
noinst_HEADERS = power.h pcmcia.h geom.h warning.h battery.h kbattery.h daemon.h notify.h apm.h version.h

EXTRA_DIST = power.xpm mini-power.xpm pcmcia.xpm mini-pcmcia.xpm laptop_settings.xpm mini-laptop_settings.xpm laptop_battery.xpm mini-laptop_battery.xpm laptop_charge.xpm mini-laptop_charge.xpm laptop_nobattery.xpm mini-laptop_nobattery.xpm laptop_nocharge.xpm mini-laptop_nocharge.xpm 

warning.cpp: warning.moc geom.h
warning.moc: warning.h
	$(MOC) warning.h -o warning.moc

daemon.cpp: daemon.moc geom.h
daemon.moc: daemon.h
	$(MOC) daemon.h -o daemon.moc

notify.cpp: notify.moc geom.h
notify.moc: notify.h
	$(MOC) notify.h -o notify.moc

kbattery.cpp: kbattery.moc geom.h
kbattery.moc: kbattery.h
	$(MOC) kbattery.h -o kbattery.moc

power.cpp: power.moc geom.h
power.moc: power.h
	$(MOC) power.h -o power.moc

battery.cpp: battery.moc geom.h
battery.moc: battery.h
	$(MOC) battery.h -o battery.moc

pcmcia.cpp: pcmcia.moc geom.h
pcmcia.moc: pcmcia.h
	$(MOC) pcmcia.h -o pcmcia.moc

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) power.xpm $(kde_icondir)/power.xpm
	$(INSTALL_DATA) pcmcia.xpm $(kde_icondir)/pcmcia.xpm
	$(INSTALL_DATA) mini-power.xpm $(kde_minidir)/power.xpm
	$(INSTALL_DATA) mini-pcmcia.xpm $(kde_minidir)/pcmcia.xpm
	$(INSTALL_DATA) laptop_settings.xpm $(kde_icondir)/laptop_settings.xpm
	$(INSTALL_DATA) mini-laptop_settings.xpm $(kde_minidir)/laptop_settings.xpm
	$(INSTALL_DATA) laptop_battery.xpm $(kde_icondir)/laptop_battery.xpm
	$(INSTALL_DATA) mini-laptop_battery.xpm $(kde_minidir)/laptop_battery.xpm
	$(INSTALL_DATA) laptop_nobattery.xpm $(kde_icondir)/laptop_nobattery.xpm
	$(INSTALL_DATA) mini-laptop_nobattery.xpm $(kde_minidir)/laptop_nobattery.xpm
	$(INSTALL_DATA) laptop_charge.xpm $(kde_icondir)/laptop_charge.xpm
	$(INSTALL_DATA) mini-laptop_charge.xpm $(kde_minidir)/laptop_charge.xpm
	$(INSTALL_DATA) laptop_nocharge.xpm $(kde_icondir)/laptop_nocharge.xpm
	$(INSTALL_DATA) mini-laptop_nocharge.xpm $(kde_minidir)/laptop_nocharge.xpm

uninstall-local:
	rm -f $(kde_icondir)/power.xpm
	rm -f $(kde_icondir)/pcmcia.xpm
	rm -f $(kde_minidir)/power.xpm
	rm -f $(kde_minidir)/pcmcia.xpm
	rm -f $(kde_icondir)/laptop_settings.xpm
	rm -f $(kde_minidir)/laptop_settings.xpm
	rm -f $(kde_icondir)/laptop_battery.xpm
	rm -f $(kde_minidir)/laptop_battery.xpm
	rm -f $(kde_icondir)/laptop_nobattery.xpm
	rm -f $(kde_minidir)/laptop_nobattery.xpm
	rm -f $(kde_icondir)/laptop_charge.xpm
	rm -f $(kde_minidir)/laptop_charge.xpm
	rm -f $(kde_icondir)/laptop_nocharge.xpm
	rm -f $(kde_minidir)/laptop_nocharge.xpm

messages:
	$(XGETTEXT) -C -ktranslate -x ../../../kdelibs/po/kde.pot $(kcmlaptop_SOURCES) && mv messages.po ../../po/kcmlaptop.pot

DISTCLEANFILES = $(BUILT_SOURCES)


