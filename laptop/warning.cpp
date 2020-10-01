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

#include "warning.h"

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
#include <sys/stat.h>
#include "version.h"

#include "geom.h"

static bool GUI;

int has_pm();
int pm_has_time();

BatteryWarning::~BatteryWarning ()
{
  if (GUI)
    {
    }
}


BatteryWarning::BatteryWarning (int t, QWidget * parent, const char *name, bool init)
    : KConfigWidget (parent, name)
{
   type = t;
  if (init)
    GUI = FALSE;
  else
    GUI = TRUE;

	// setup the tabs


  pm = ::has_pm();

  config = kapp->getConfig();
  GetSettings(0);
  if (GUI)
    if (!pm) {
      QVBoxLayout *top_layout = new QVBoxLayout(this, 12, 5);

      QLabel* explain = new QLabel(i18n("Your computer doesn't have the Linux PM (Advanced\nPower Management) software installed, or doesn't\nhave the PM kernel drivers installed - click the\n'Help' button below for more information on how to\nobtain this software"), this);
      explain->setMinimumSize(explain->sizeHint());
      top_layout->addWidget(explain, 0);

      top_layout->addStretch(1);

      top_layout->activate();

    } else {
	QVBoxLayout *top_layout = new QVBoxLayout(this, 10, 5);

	QGridLayout *grid = new QGridLayout(6, 6, 5);
	top_layout->addLayout(grid);

	grid->addColSpacing(2, 5);
	grid->addColSpacing(5, 5);

	QLabel* low_label = 0;
	if (type) {
		if (have_time == 1) {
			low_label = new QLabel(i18n("Critical trigger (minutes):"), this);
		} else {
			low_label = new QLabel(i18n("Critical trigger (percent):"), this);
		}
	} else {
		if (have_time == 1) {
			low_label = new QLabel(i18n("Low trigger (minutes):"), this);
		} else {
			low_label = new QLabel(i18n("Low trigger (percent):"), this);
		}
	}
	low_label->setMinimumSize(low_label->sizeHint());
	grid->addWidget(low_label, 0, 0);


        editLow = new QLineEdit(this);
        editLow->setMinimumSize(editLow->sizeHint());
        grid->addWidget(editLow, 0, 1);   


	// setup the Run Command stuff
	checkRunCommand = new QCheckBox(i18n("Run Command"), this);
	checkRunCommand->setMinimumSize(checkRunCommand->sizeHint());
	grid->addWidget(checkRunCommand, 1, 0);

	editRunCommand = new QLineEdit(this);
	editRunCommand->setMinimumSize(editRunCommand->sizeHint());
	grid->addWidget(editRunCommand, 2, 0);

	buttonBrowseRunCommand = new QPushButton(i18n("Browse"), this);
	buttonBrowseRunCommand->setMinimumSize(75, 25);
	buttonBrowseRunCommand->setMaximumSize(buttonBrowseRunCommand->sizeHint());
	grid->addWidget(buttonBrowseRunCommand, 2, 1);

	// setup the Play Sound stuff
	checkPlaySound = new QCheckBox(i18n("Play Sound"), this);
	checkPlaySound->setMinimumSize(checkPlaySound->sizeHint());
	grid->addWidget(checkPlaySound, 3, 0);

	editPlaySound = new QLineEdit(this);
	editPlaySound->setMinimumSize(editPlaySound->sizeHint());
	grid->addWidget(editPlaySound, 4, 0);

	buttonBrowsePlaySound = new QPushButton(i18n("Browse"), this);
	buttonBrowsePlaySound->setMinimumSize(75, 25);
	buttonBrowsePlaySound->setMaximumSize(buttonBrowsePlaySound->sizeHint());
	grid->addWidget(buttonBrowsePlaySound, 4, 1);

	// setup the System Sound stuff
	checkBeep = new QCheckBox(i18n("System Beep"), this);
	checkBeep->setMinimumSize(checkBeep->sizeHint());
	top_layout->addWidget(checkBeep);

	// setup the System Sound stuff
	checkNotify = new QCheckBox(i18n("Notify"), this);
	checkNotify->setMinimumSize(checkNotify->sizeHint());
	top_layout->addWidget(checkNotify);

	int can_suspend = 1;
	struct stat s;
        if (stat("/usr/bin/pm", &s) || !(getuid() == 0 || s.st_mode&S_ISUID)) {
		can_suspend = 0;
		checkSuspend = NULL;
		checkStandby = NULL;
	} else {
		checkSuspend = new QCheckBox(i18n("Suspend"), this);
		checkSuspend->setMinimumSize(checkSuspend->sizeHint());
		top_layout->addWidget(checkSuspend);

		checkStandby = new QCheckBox(i18n("Standby"), this);
		checkStandby->setMinimumSize(checkStandby->sizeHint());
		top_layout->addWidget(checkStandby);
	}


        QLabel* explain = 0;

	if (type) {
		explain = new QLabel(i18n("This panel controls how and when you receive warnings\nthat your battery power is going to run out VERY VERY soon"), this);
	} else {
		explain = new QLabel(i18n("This panel controls how and when you receive warnings\nthat your battery power is about to run out"), this);
	}
        explain->setMinimumSize(explain->sizeHint());
        top_layout->addWidget(explain, 0);

	if (!can_suspend) {
        	QLabel* note = new QLabel(i18n("\nIf you make /usr/bin/pm setuid then you will also\nbe able to choose 'susspend' and 'standby' in the\nabove dialog - check out the help button below to\nfind out how to do this"), this);
        	note->setMinimumSize(note->sizeHint());
        	top_layout->addWidget(note, 0);
		
	}

	top_layout->addStretch(1);


	QHBoxLayout *v1 = new QHBoxLayout;
        top_layout->addLayout(v1, 0);
	v1->addStretch(1);
	QString s1 = LAPTOP_VERSION;
	QString s2 = i18n("Version: ")+s1;
        QLabel* vers = new QLabel(s2, this);
	vers->setMinimumSize(vers->sizeHint());
        v1->addWidget(vers, 0);
	
	// connect some slots and signals
	connect(buttonBrowsePlaySound, SIGNAL(clicked()),
	                                 SLOT(browsePlaySound()));
	connect(buttonBrowseRunCommand, SIGNAL(clicked()),
	                                  SLOT(browseRunCommand()));
	connect(checkPlaySound, SIGNAL(toggled(bool)),
	                          SLOT(enablePlaySound(bool)));
	connect(checkRunCommand, SIGNAL(toggled(bool)),
	                           SLOT(enableRunCommand(bool)));

	top_layout->activate();


    }
  GetSettings(1);

}

void BatteryWarning::resizeEvent(QResizeEvent *)
{
}

void BatteryWarning::GetSettings( int x )
{

	// open the config file
	if (!x) {
		config->setGroup((type?"BatteryCritical":"BatteryLow"));

		low_val = config->readEntry("LowVal", (type?"5":"15"));
		runcommand = config->readBoolEntry("RunCommand", false);
		playsound = config->readBoolEntry("PlaySound", false);
		beep = config->readBoolEntry("SystemBeep", true);
		notify = config->readBoolEntry("Notify", (type && checkSuspend ? false : true));
		do_suspend = config->readBoolEntry("Suspend", (type && checkSuspend ? true :false));
		do_standby = config->readBoolEntry("Standby", false);
		runcommand_val = config->readEntry("RunCommandPath");
		sound_val = config->readEntry("PlaySoundPath");
		have_time = config->readNumEntry("HaveTime", 2);
		if (pm_has_time())
			have_time = 1;
	
	} else
	if (GUI && pm) {
		checkRunCommand->setChecked(runcommand);
		checkPlaySound->setChecked(playsound);
		checkBeep->setChecked(beep);
		checkNotify->setChecked(notify);
		if (checkStandby)
			checkStandby->setChecked(do_standby);
		if (checkSuspend)
			checkSuspend->setChecked(do_suspend);
		editRunCommand->setText(runcommand_val);
		editLow->setText(low_val);
		editPlaySound->setText(sound_val);
	
		enableRunCommand(checkRunCommand->isChecked());
		enablePlaySound(checkPlaySound->isChecked());
	}
}

void BatteryWarning::saveParams( void )
{
    if (GUI && pm) {
    	runcommand = checkRunCommand->isChecked();
    	playsound = checkPlaySound->isChecked();
    	beep = checkBeep->isChecked();
    	notify = checkNotify->isChecked();
    	do_suspend = (checkSuspend? checkSuspend->isChecked() : false);
    	do_standby = (checkStandby? checkStandby->isChecked() : false);
    	runcommand_val = editRunCommand->text();
    	low_val = editLow->text();
    	sound_val = editPlaySound->text();
		
    }
    config->setGroup((type?"BatteryCritical":"BatteryLow"));
       
    config->writeEntry("LowVal", low_val);
    config->writeEntry("RunCommand", runcommand);
    config->writeEntry("PlaySound", playsound);
    config->writeEntry("SystemBeep", beep);
    config->writeEntry("Notify", notify);
    config->writeEntry("Suspend", do_suspend);
    config->writeEntry("Standby", do_standby);
    config->writeEntry("RunCommandPath", runcommand_val);
    config->writeEntry("PlaySoundPath", sound_val);
    config->sync();
}

void BatteryWarning::loadSettings()
{
  GetSettings(1);
}

void BatteryWarning::applySettings()
{
  saveParams();
}

void BatteryWarning::defaultSettings()
{
	// open the config file
	runcommand = 0;
	playsound = 0;
	beep = 1;
	notify = (type && checkSuspend ? 0:1);
	do_standby = 0;
	do_suspend = (type && checkSuspend ? 1:0);
	runcommand_val = "";
	low_val = (type?"5":"15");
	sound_val = "";
	

	if (GUI && pm) {
		checkRunCommand->setChecked(runcommand);
		checkPlaySound->setChecked(playsound);
		checkBeep->setChecked(beep);
		checkNotify->setChecked(notify);
		if (checkStandby)
			checkStandby->setChecked(do_standby);
		if (checkSuspend)
			checkSuspend->setChecked(do_suspend);
		editRunCommand->setText(runcommand_val);
		editLow->setText(low_val);
		editPlaySound->setText(sound_val);
	
		enableRunCommand(checkRunCommand->isChecked());
		enablePlaySound(checkPlaySound->isChecked());
	}
}


#include "warning.moc"

const int BatteryWarning::getHaveTime() const
{
	return have_time;
}

const bool BatteryWarning::getRunCommand() const
{
	return runcommand;
}

const unsigned int BatteryWarning::getLowValue() const
{
	return low_val.toInt();
}

const char* BatteryWarning::getRunCommandPath() const
{
	return runcommand_val.data();
}

const bool BatteryWarning::getPlaySound() const
{
	return playsound;
}

const char* BatteryWarning::getPlaySoundPath() const
{
	return sound_val.data();
}

const bool BatteryWarning::getSystemBeep() const
{
	return beep;
}

const bool BatteryWarning::getSuspend() const
{
	return do_suspend;
}

const bool BatteryWarning::getStandby() const
{
	return do_standby;
}

const bool BatteryWarning::getNotify() const
{
	return notify;
}

void BatteryWarning::enableRunCommand(bool enable)
{
	editRunCommand->setEnabled(enable);
	buttonBrowseRunCommand->setEnabled(enable);
}

void BatteryWarning::enablePlaySound(bool enable)
{
	editPlaySound->setEnabled(enable);
	buttonBrowsePlaySound->setEnabled(enable);
}

void BatteryWarning::browseRunCommand()
{
	QString command_path = KFileDialog::getOpenFileName();
	if (!command_path.isEmpty() && !command_path.isNull())
	{
		editRunCommand->setText(command_path);
	}
}

void BatteryWarning::browsePlaySound()
{
	QString sound_path = KFileDialog::getOpenFileName();
	if (!sound_path.isEmpty() && !sound_path.isNull())
	{
		editPlaySound->setText(sound_path);
	}
}

