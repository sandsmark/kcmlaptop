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

#include <iostream.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <qfileinf.h> 
#include <qstring.h>
#include <qmsgbox.h> 

#include <kmsgbox.h> 
#include <qlayout.h>
#include "power.h"

#include <X11/Xlib.h>

#include "geom.h"
#include "version.h"

static bool GUI;

int has_pm();

PowerConfig::~PowerConfig ()
{
}

PowerConfig::PowerConfig (QWidget * parent, const char *name, bool init)
    : KConfigWidget (parent, name)
{
   editwait = 0;
   powerOff = 0;
   nopowerOff = 0;
  if (init)
    GUI = FALSE;
  else
    GUI = TRUE;

  pm = ::has_pm();

  if (GUI)
    if (!pm) {
      QVBoxLayout *top_layout = new QVBoxLayout(this, 12, 5);

      QLabel* explain = new QLabel(i18n("Your computer doesn't have the Linux PM (Advanced\nPower Management) software installed, or doesn't\nhave the PM kernel drivers installed - click the\n'Help' button below for more information on how to\nobtain this software"), this);
      explain->setMinimumSize(explain->sizeHint());
      top_layout->addWidget(explain, 0);

      top_layout->addStretch(1);

      top_layout->activate();

    } else {
     struct stat s;

     if (stat("/usr/bin/pm", &s) || !(getuid() == 0 || s.st_mode&S_ISUID)) 
	pm = 0;
     if (!pm) {
      QVBoxLayout *top_layout = new QVBoxLayout(this, 12, 5);

      QLabel* explain = new QLabel(i18n("Automatic suspend/standby functionality is only\nenabled if you have installed the pmd package and\nyou have set /usr/bin/pm as setuid root - to do\nthis log on as root and issue the shell command\n'chown root /usr/bin/pm;chmod +s /usr/bin/pm' "), this);
      explain->setMinimumSize(explain->sizeHint());
      top_layout->addWidget(explain, 0);

      top_layout->addStretch(1);

      top_layout->activate();
     } else {
      QVBoxLayout *top_layout = new QVBoxLayout(this, 12, 5);

      QHBoxLayout *box0 = new QHBoxLayout;
      top_layout->addLayout(box0, 0);

      QGroupBox* box1 = new QGroupBox(i18n("Not Powered:"), this);
      box0->addWidget(box1, 0);
      QVBoxLayout *a0 = new QVBoxLayout(box1, 20, 3);
      a0->addSpacing(8);

      nopowerBox = new QButtonGroup("", 0, "disconnect");
      nopowerSuspend = new QRadioButton(klocale->translate("Suspend"), box1, "nosuspend");
      nopowerSuspend->setMinimumSize(nopowerSuspend->sizeHint());
      a0->addWidget(nopowerSuspend, 0);
      nopowerBox->insert(nopowerSuspend);
      nopowerStandby = new QRadioButton(klocale->translate("Standby"), box1, "nostandby");
      nopowerStandby->setMinimumSize(nopowerStandby->sizeHint());
      a0->addWidget(nopowerStandby, 0);
      nopowerBox->insert(nopowerStandby);
      nopowerOff = new QRadioButton(klocale->translate("Off"), box1, "nooff");
      nopowerOff->setMinimumSize(nopowerOff->sizeHint());
      a0->addWidget(nopowerOff, 0);
      nopowerBox->insert(nopowerOff);

      QLabel* noedlabel = new QLabel(i18n("Wait for (mins):"), box1);
      noedlabel->setMinimumSize(noedlabel->sizeHint());
      a0->addWidget(noedlabel, 0);

      noeditwait = new QLineEdit(box1);
      noeditwait->setMinimumSize(noeditwait->sizeHint());
      a0->addWidget(noeditwait, 0);   
      
      //nopowerSuspend->adjustSize();
      //nopowerStandby->adjustSize();
      //nopowerOff->adjustSize();


      QGroupBox* box2 = new QGroupBox(i18n("Powered:"), this);
      box0->addWidget(box2, 0);
      QVBoxLayout *a1 = new QVBoxLayout(box2, 20, 3);
      a1->addSpacing(8);



      powerBox = new QButtonGroup("", 0, "connect");
      powerSuspend = new QRadioButton(klocale->translate("Suspend"), box2, "suspend");
      powerSuspend->setMinimumSize(powerSuspend->sizeHint());
      a1->addWidget(powerSuspend, 0);
      powerBox->insert(powerSuspend);
      powerStandby = new QRadioButton(klocale->translate("Standby"), box2, "standby");
      powerStandby->setMinimumSize(powerStandby->sizeHint());
      a1->addWidget(powerStandby, 0);
      powerBox->insert(powerStandby);
      powerOff = new QRadioButton(klocale->translate("Off"), box2, "off");
      powerOff->setMinimumSize(powerOff->sizeHint());
      a1->addWidget(powerOff, 0);
      powerBox->insert(powerOff);

      QLabel* edlabel = new QLabel(i18n("Wait for (mins):"), box2);
      edlabel->setMinimumSize(edlabel->sizeHint());
      a1->addWidget(edlabel, 0);

      editwait = new QLineEdit(box2);
      editwait->setMinimumSize(editwait->sizeHint());
      a1->addWidget(editwait, 0);   
      
      
      //powerSuspend->adjustSize();
      //powerStandby->adjustSize();
      //powerOff->adjustSize();
      box0->addStretch(1);

      QLabel* explain = new QLabel(i18n("This panel configures the behaviour of the automatic\npower-down feature - it works as a sort of extreme\nscreen-saver, you can configure different timeouts\nand behaviours depending on whether or not your\nlaptop is plugged in to the power.\n"), this);
      explain->setMinimumSize(explain->sizeHint());
      top_layout->addWidget(explain, 0);

      QLabel* explain3 = new QLabel(i18n("Different laptops may respond to 'standby' in\ndifferent ways - in many it's only a temporary\nstate and may not be usefull for you."), this);
      explain3->setMinimumSize(explain3->sizeHint());
      top_layout->addWidget(explain3, 0);

      top_layout->addStretch(1);

	QHBoxLayout *v1 = new QHBoxLayout;
        top_layout->addLayout(v1, 0);
	v1->addStretch(1);
	QString s1 = LAPTOP_VERSION;
	QString s2 = i18n("Version: ")+s1;
        QLabel* vers = new QLabel(s2, this);
        vers->setMinimumSize(vers->sizeHint());
        v1->addWidget(vers, 0);
	

      top_layout->activate();
    }
 }

  config = kapp->getConfig();

  GetSettings();
}

void PowerConfig::resizeEvent(QResizeEvent *)
{

}

int  PowerConfig::getNoPower()
{
	if (!GUI || !pm || !nopowerOff)
		return(nopower);
  if (nopowerOff->isChecked())
    return 0;
  else
  if (nopowerStandby->isChecked())
    return 1;
  else
    return 2;
}

int  PowerConfig::getPower()
{
	if (!GUI || !pm || !powerOff)
		return(power);
  if (powerOff->isChecked())
    return 0;
  else
  if (powerStandby->isChecked())
    return 1;
  else
    return 2;
}

// set the slider and LCD values
void PowerConfig::setPower(int p, int np)
{
  if (!pm || nopowerOff == 0)
	return;
  nopowerSuspend->setChecked(FALSE);
  nopowerStandby->setChecked(FALSE);
  nopowerOff->setChecked(FALSE);
  switch (np) {
  case 0: nopowerOff->setChecked(TRUE);break;
  case 1: nopowerStandby->setChecked(TRUE);break;
  case 2: nopowerSuspend->setChecked(TRUE);break;
  }
  powerSuspend->setChecked(FALSE);
  powerStandby->setChecked(FALSE);
  powerOff->setChecked(FALSE);
  switch (p) {
  case 0: powerOff->setChecked(TRUE);break;
  case 1: powerStandby->setChecked(TRUE);break;
  case 2: powerSuspend->setChecked(TRUE);break;
  }
}

void PowerConfig::GetSettings( void )
{

  config->setGroup("LaptopPower");
  nopower = config->readNumEntry("NoPowerSuspend", 1);
  power = config->readNumEntry("PowerSuspend", 0);
  edit_wait = config->readEntry("PowerWait", "20");
  noedit_wait = config->readEntry("NoPowerWait", "5");
  
  // the GUI should reflect the real values
  if (GUI && editwait) {
	editwait->setText(edit_wait);
	noeditwait->setText(noedit_wait);
   	setPower(power, nopower);
  }
}

void PowerConfig::saveParams( void )
{
  if (GUI && editwait)
    {
      	power = getPower();
      	nopower = getNoPower();
	edit_wait = editwait->text();
	noedit_wait = noeditwait->text();
    }

  config->setGroup("LaptopPower");
  config->writeEntry("NoPowerSuspend", nopower);
  config->writeEntry("PowerSuspend", power);
  config->writeEntry("PowerWait", edit_wait);
  config->writeEntry("NoPowerWait", noedit_wait);
  config->sync();
}

void PowerConfig::loadSettings()
{
  GetSettings();
}

void PowerConfig::applySettings()
{
  saveParams();
}

void PowerConfig::defaultSettings()
{
  setPower(1, 0);
  edit_wait = "20";
  noedit_wait = "5";
  if (GUI && editwait) {
	editwait->setText(edit_wait);
        noeditwait->setText(noedit_wait);
  }
}


const int PowerConfig::getNoEditWait() const
{
	return noedit_wait.toInt();
}

const int PowerConfig::getEditWait() const
{
	return edit_wait.toInt();
}

#include "power.moc"
