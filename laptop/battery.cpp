/*
 * power.cpp
 * Copyright (c) 1999 Paul Campbell paul@taniwha.com
 *
 *
 * stolen from keyboard.cpp
 * Copyright (c) 1997 Patrick Dowler dowler@morgul.fsh.uvic.ca
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "battery.h"

#if QT_VERSION < 140
#include <qgrpbox.h>
#include <qfileinf.h>
#include <qmsgbox.h>
#include <qlined.h>
#include <qchkbox.h>
#include <qpushbt.h>
#include <qcombo.h>
#else
#include <qgroupbox.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qheader.h>
#endif // QT_VERSION < 140

#include <qpixmap.h>
#include <qfont.h>
#include <qlabel.h>
#include <qstrlist.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qdict.h>
#include <qlist.h>

#include <kfiledialog.h>
#include <kapp.h>
#include <ktabctl.h>
#include <ksimpleconfig.h>
#include <kfm.h>
#include <kprocess.h>

#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#include "geom.h"

#include "version.h"

#include "pm.h"

static bool GUI;

int has_pm();

BatteryConfig::~BatteryConfig ()
{
  if (GUI)
    {
    }
}

BatteryConfig::BatteryConfig (QWidget * parent, const char *name, bool init)
    : KConfigWidget (parent, name)
{
  if (init)
    GUI = FALSE;
  else
    GUI = TRUE;

  pm = ::has_pm();

  if (GUI)
    if (!pm) {
      QVBoxLayout *top_layout = new QVBoxLayout(this, 12, 5);

      QLabel* explain = new QLabel(i18n("Your computer doesn't have the Linux PM (Advanced\nPower Management) software installed, or doesn't have\nthe PM kernel drivers installed - click the 'Help'\nbutton below for more information on how to obtain\nthis software"), this);
      explain->setMinimumSize(explain->sizeHint());
      top_layout->addWidget(explain, 0);

      top_layout->addStretch(1);

      top_layout->activate();

    } else {
	QVBoxLayout *top_layout = new QVBoxLayout(this, 12, 5);
	QGridLayout *top_grid = new QGridLayout(3, 3);
	top_layout->addLayout(top_grid);

	// do we show the monitor
	runMonitor = new QCheckBox(i18n("Show Battery Monitor"), this);
        runMonitor->setFixedSize(200, 24);
        top_grid->addWidget(runMonitor, 0, 0);   

	// do we dock automatically?
	checkDock = new QCheckBox(i18n("Dock in panel"), this);
        checkDock->setFixedSize(200, 24);
        top_grid->addWidget(checkDock, 1, 0);   

	top_grid->setColStretch(2, 1);

	// the poll time (in seconds)
	QLabel* poll_label = new QLabel(i18n("Poll (sec):"), this);
        poll_label->setFixedSize(200, 24);
	top_grid->addWidget(poll_label, 2, 0);


        editPoll = new QLineEdit(this);
        editPoll->setFixedSize(60, 24);
        top_grid->addWidget(editPoll, 2, 1);   

	QHBoxLayout *xx = new QHBoxLayout;
	top_layout->addLayout(xx);

	// group box to hold the icons together
	QGroupBox* icons_groupbox = new QGroupBox(i18n("Icons:"), this);
	xx->addWidget(icons_groupbox, 0);
	xx->addStretch(1);

	// layout to hold the icons inside the groupbox
	QVBoxLayout *icon_layout = new QVBoxLayout(icons_groupbox, 8);
	icon_layout->addSpacing(8);

	QGridLayout *icon_grid = new QGridLayout(2, 3);

	icon_layout->addLayout(icon_grid);

	QHBoxLayout *a00 = new QHBoxLayout;
	QLabel* nobattery_label = new QLabel(i18n("No Battery:"), icons_groupbox);
	nobattery_label->setFixedSize(100, 24);
	icon_grid->addLayout(a00,0,0);
	a00->addStretch(1);
	a00->addWidget(nobattery_label,0);
	a00->addStretch(1);

	QHBoxLayout *a01 = new QHBoxLayout;
	QLabel* nocharge_label = new QLabel(i18n("Not Charging:"), icons_groupbox);
	nocharge_label->setFixedSize(100, 24);
	icon_grid->addLayout(a01, 0, 1);
	a01->addStretch(1);
	a01->addWidget(nocharge_label,0);
	a01->addStretch(1);

	QHBoxLayout *a02 = new QHBoxLayout;
	QLabel* charging_label = new QLabel(i18n("Charging:"), icons_groupbox);
	charging_label->setFixedSize(100, 24);
	icon_grid->addLayout(a02, 0, 2);
	a02->addStretch(1);
	a02->addWidget(charging_label,0);
	a02->addStretch(1);

	QHBoxLayout *a10 = new QHBoxLayout;
	buttonNoBattery = new KIconLoaderButton(icons_groupbox);
	buttonNoBattery->setFixedSize(50, 50);
	icon_grid->addLayout(a10, 1, 0);
	a10->addStretch(1);
	a10->addWidget(buttonNoBattery,0);
	a10->addStretch(1);

	QHBoxLayout *a11 = new QHBoxLayout;
	buttonNoCharge = new KIconLoaderButton(icons_groupbox);
	buttonNoCharge->setFixedSize(50, 50);
	icon_grid->addLayout(a11, 1, 1);
	a11->addStretch(1);
	a11->addWidget(buttonNoCharge,0);
	a11->addStretch(1);

	QHBoxLayout *a12 = new QHBoxLayout;
	buttonCharge = new KIconLoaderButton(icons_groupbox);
	buttonCharge->setFixedSize(50, 50);
	icon_grid->addLayout(a12, 1, 2);
	a12->addStretch(1);
	a12->addWidget(buttonCharge,0);
	a12->addStretch(1);

        QLabel* explain = new QLabel(i18n("This panel controls whether the battery status\nmonitor appears in the dock and what it looks like"), this);
        explain->setMinimumSize(explain->sizeHint());
        top_layout->addWidget(explain, 0);

	top_layout->addStretch(1);


	QHBoxLayout *v1 = new QHBoxLayout;
        top_layout->addLayout(v1, 0);
	v1->addStretch(1);

        QString s2;
        pm_info info;
        if (pm_read(&info) || (info.pm_flags&PM_NOT_AVAILABLE)) {
            QString s1 = LAPTOP_VERSION;
            s2 = i18n("Version: ")+s1;
        } else {
            QString timeString;
            if (info.battery_time > 0) {
                QTime timeLeft(info.battery_time/3600, (info.battery_time / 60) % 60, info.battery_time % 60);
                timeString = timeLeft.toString() + " left";
            } else {
                if (info.ac_line_status) {
                    timeString = "not charging";
                } else {
                    timeString = "";
                }
            }
            s2.sprintf("%s, %s\nVersion: %s", info.ac_line_status ?
                    i18n("Plugged in") : i18n("Running on batteries"),
                    timeString.data(),
                    LAPTOP_VERSION);
        }
        QLabel* vers = new QLabel(s2, this);
        vers->setMinimumSize(vers->sizeHint());
        v1->addWidget(vers, 0);
	

	top_layout->activate();
    }

  config = kapp->getConfig();

  GetSettings();
}

void BatteryConfig::resizeEvent(QResizeEvent *)
{

}

void BatteryConfig::GetSettings( void )
{

	config->setGroup("BatteryDefault");

	poll_time = config->readEntry("Poll", "20");
	docked = config->readBoolEntry("Docked", true);
	enablemonitor = config->readBoolEntry("Enable", true);

	nobattery = config->readEntry("NoBatteryPixmap", "laptop_nobattery.xpm");
	nochargebattery = config->readEntry("NoChargePixmap", "laptop_nocharge.xpm");
	chargebattery = config->readEntry("ChargePixmap", "laptop_charge.xpm");

	if (GUI &&pm) {
		editPoll->setText(poll_time);
		buttonNoCharge->setIcon(nochargebattery);
		buttonNoCharge->setPixmap(ICON(nochargebattery));
		buttonCharge->setIcon(chargebattery);
		buttonCharge->setPixmap(ICON(chargebattery));
		buttonNoBattery->setIcon(nobattery);
		buttonNoBattery->setPixmap(ICON(nobattery));
		checkDock->setChecked(docked);
		runMonitor->setChecked(enablemonitor);
	}
}

void BatteryConfig::saveParams( void )
{

	if (GUI && pm) {
		poll_time = editPoll->text();
		docked = checkDock->isChecked();
		enablemonitor = runMonitor->isChecked();
		nobattery =  buttonNoBattery->icon();
		chargebattery =  buttonCharge->icon();
		nochargebattery =  buttonNoCharge->icon();
	}
	config->setGroup("BatteryDefault");

	config->writeEntry("Enable", enablemonitor);
	config->writeEntry("Poll", poll_time);
	config->writeEntry("Docked", docked);
	config->writeEntry("NoBatteryPixmap", nobattery);
	config->writeEntry("ChargePixmap", chargebattery);
	config->writeEntry("NoChargePixmap", nochargebattery);
    config->sync();

}

void BatteryConfig::loadSettings()
{
  GetSettings();
}

void BatteryConfig::applySettings()
{
  saveParams();
}

void BatteryConfig::defaultSettings()
{
	poll_time = "20";
	docked = true;
	enablemonitor = false;

	nobattery = "laptop_nobattery.xpm";
	nochargebattery = "laptop_nocharge.xpm";
	chargebattery = "laptop_charge.xpm";

	if (GUI && pm) {
		editPoll->setText(poll_time);
		buttonNoCharge->setIcon(nochargebattery);
		buttonNoCharge->setPixmap(ICON(nochargebattery));
		buttonCharge->setIcon(chargebattery);
		buttonCharge->setPixmap(ICON(chargebattery));
		buttonNoBattery->setIcon(nobattery);
		buttonNoBattery->setPixmap(ICON(nobattery));
		checkDock->setChecked(docked);
		runMonitor->setChecked(enablemonitor);
	}
}


#include "battery.moc"

const bool BatteryConfig::getDock() const
{
	return docked;
}

const char* BatteryConfig::getButtonNoCharge() const
{
	return nochargebattery.data();
}

const char* BatteryConfig::getButtonCharge() const
{
	return chargebattery.data();
}

const char* BatteryConfig::getButtonNoBattery() const
{
	return nobattery.data();
}

const int BatteryConfig::getPoll() const
{
	return poll_time.toInt();
}

const bool BatteryConfig::getEnabled() const
{
	return enablemonitor;
}


KBatteryNewDlg::KBatteryNewDlg(QWidget* parent, const char* name)
	: QDialog(parent, name, true, 0)
{
	QGridLayout *layout = new QGridLayout(this, 2, 2, 12, 5);
	layout->activate();
	
	QLabel* label1 = new QLabel(i18n("New Name:"), this);
	label1->setMinimumSize(label1->sizeHint());
	layout->addWidget(label1, 0, 0);

	editName = new QLineEdit(this);
	editName->setFocus();
	editName->setMinimumSize(editName->sizeHint());
	layout->addWidget(editName, 0, 1, 1);

	QBoxLayout *buttons = new QBoxLayout(QBoxLayout::LeftToRight);
	layout->addLayout(buttons, 1, 1);

	// ok button
	QPushButton* button_ok = new QPushButton(i18n("OK"), this);
	connect(button_ok, SIGNAL(clicked()), SLOT(accept()));
	button_ok->setDefault(true);
	button_ok->setMinimumSize(button_ok->sizeHint());
	buttons->addWidget(button_ok);

	// cancel button
	QPushButton* button_cancel = new QPushButton(i18n("Cancel"), this);
	button_cancel->setMinimumSize(button_cancel->sizeHint());
	connect(button_cancel, SIGNAL(clicked()), SLOT(reject()));
	buttons->addWidget(button_cancel);

	// set my name
	setCaption(i18n("New Name"));
}
