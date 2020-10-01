Name: kcmlaptop
Summary: KDE Laptop Control Panel
Version: 0.82
Release: 1
Copyright: GPL
Group: X11/KDE/System
Source: www.taniwha.com/laptop/kcmlaptop-0.82.tar.gz
URL: http://www.taniwha.com/laptop
Distribution: KDE
Packager: Paul Campbell <paul@taniwha.com>

%description
A KDE control panel for Laptops - 
puts a battery meter icon in the control strip, also
allows you to set up events to occur when the battery power
is running low.

%prep
%setup -n kcmlaptop-0.82

%build
export KDEDIR=/opt/kde
./configure --prefix=$KDEDIR
make

%install
make install

%files
/opt/kde/bin/kcmlaptop
/opt/kde/share/doc/HTML/en/kcontrol/kcmlaptop/
/opt/kde/share/applnk/Settings/Laptop/
/opt/kde/share/icons/battery.xpm
/opt/kde/share/icons/power.xpm
/opt/kde/share/icons/pcmcia.xpm
/opt/kde/share/icons/laptop_settings.xpm
/opt/kde/share/icons/laptop_nobattery.xpm
/opt/kde/share/icons/laptop_nocharge.xpm
/opt/kde/share/icons/laptop_charge.xpm
/opt/kde/share/icons/mini/battery.xpm
/opt/kde/share/icons/mini/power.xpm
/opt/kde/share/icons/mini/pcmcia.xpm
/opt/kde/share/icons/mini/laptop_settings.xpm
/opt/kde/share/icons/mini/laptop_nobattery.xpm
/opt/kde/share/icons/mini/laptop_nocharge.xpm
/opt/kde/share/icons/mini/laptop_charge.xpm
