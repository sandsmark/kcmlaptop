/*
  main.cpp - A sample KControl Application

  written 1997 by Matthias Hoelzer
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   
  */


#include <unistd.h>
#include <kcontrol.h>
#include "power.h"
#include "pcmcia.h"
#include "warning.h"
#include "battery.h"
#include "daemon.h"

#include "apm.h"

static int apm_no_time;
int has_apm()
{
	static int init = 0;
	static int val;
	if (init)
		return(val);

	init = 1;
	val = 1;
	apm_no_time=0;
	apm_info x = {{10*0},0,0,0,0,0,0,0,0,0};
	if (apm_read(&x) || (x.apm_flags&0x20)) {
		val = 0;
		apm_no_time = 1;
	} else {
		apm_no_time = x.battery_time < 0;
	}
 	return(val);
}

int apm_has_time()
{
 	return(!apm_no_time);
}


class KInputApplication : public KControlApplication
{
public:

  KInputApplication(int &argc, char **arg, const char *name);

  void init();
  void apply();
  void defaultValues();
  void check();

private:

  PcmciaConfig *pcmcia;
  PowerConfig *power;
  BatteryWarning *warning;
  BatteryWarning *critical;
  BatteryConfig *battery;
  BatteryDaemon *daemon;
};


KInputApplication::KInputApplication(int &argc, char **argv, const char *name)
  : KControlApplication(argc, argv, name)
{
  battery = 0; pcmcia = 0; power = 0; critical = 0; warning = 0;
  daemon = new BatteryDaemon();

  if (runGUI())
    {
      if (!pages || pages->contains("battery"))
	addPage(battery = new BatteryConfig(dialog, "battery", FALSE), 
		klocale->translate("&Battery"), "laptop-3.html");
      if (!pages || pages->contains("pcmcia"))
	addPage(pcmcia = new PcmciaConfig(dialog, "pcmcia", FALSE), 
		klocale->translate("&Pcmcia"), "laptop-5.html");
      if (!pages || pages->contains("power"))
	addPage(power = new PowerConfig(dialog, "power", FALSE), 
		klocale->translate("&Power"), "laptop-6.html");

      if (!pages || pages->contains("warning"))
	addPage(warning = new BatteryWarning(0, dialog, "warning", FALSE), 
		klocale->translate("&Warning"), "laptop-4.html");

      if (!pages || pages->contains("critical"))
	addPage(critical = new BatteryWarning(1, dialog, "critical", FALSE), 
		klocale->translate("&Critical"), "laptop-4.html");

      if (battery || pcmcia || power || warning || critical)
        dialog->show();
      else
        {
          fprintf(stderr, klocale->translate("usage: kcmlaptop [-init | {pcmcia,power,warning,critical,battery}]\n"));
          justInit = TRUE;
        }

    }
}


void KInputApplication::init()
{
  sleep(10);	// don't show the docked icon 'till we've be going for a while
  
  
  PcmciaConfig *pcmcia = new PcmciaConfig(0, 0, TRUE);
  pcmcia->applySettings();
  delete pcmcia;

  PowerConfig *power = new PowerConfig(0, 0, TRUE);
  power->applySettings();

  BatteryWarning *warning = new BatteryWarning(0, 0, 0, TRUE);
  warning->applySettings();

  BatteryWarning *critical = new BatteryWarning(1, 0, 0, TRUE);
  critical->applySettings();

  BatteryConfig *battery = new BatteryConfig(0, 0, TRUE);
  battery->applySettings();

  check();

  delete power;
  delete warning;
  delete critical;
  delete battery;
}


void KInputApplication::apply()
{
  if (battery)
    battery->applySettings();
  if (pcmcia)
    pcmcia->applySettings();
  if (power)
    power->applySettings();
  if (warning)
    warning->applySettings();
  if (critical)
    critical->applySettings();
  check();
}


void KInputApplication::check()
{
	if (battery == NULL)
		battery = new BatteryConfig(0, 0, TRUE);
	if (warning == NULL)
		warning = new BatteryWarning(0, 0, 0, TRUE);
	if (critical == NULL)
		critical = new BatteryWarning(1, 0, 0, TRUE);
	if (power == NULL)
		power = new PowerConfig(0, 0, TRUE);
	bool should_run = 0;
	should_run |= battery->getEnabled();
	should_run |= critical->getRunCommand();
	should_run |= critical->getPlaySound();
	should_run |= critical->getSystemBeep();
	should_run |= critical->getNotify();
	should_run |= critical->getSuspend();
	should_run |= critical->getStandby();
	should_run |= warning->getRunCommand();
	should_run |= warning->getPlaySound();
	should_run |= warning->getSystemBeep();
	should_run |= warning->getNotify();
	should_run |= warning->getSuspend();
	should_run |= warning->getStandby();
	should_run |= power->getPower() != 0;
	should_run |= power->getNoPower() != 0;
	if (!has_apm())
		should_run = 0;
	if (!should_run) {
		daemon->kill();
	} else {
		daemon->restart();
	}
}

void KInputApplication::defaultValues()
{
  if (battery)
    battery->defaultSettings();
  if (pcmcia)
    pcmcia->defaultSettings();
  if (power)
    power->defaultSettings();
  if (warning)
    warning->defaultSettings();
  if (critical)
    critical->defaultSettings();
}


extern int bmain(int argc, char *argv[]);

int main(int argc, char **argv)
{
	
   if (argc > 1) {
	for (int i = 1; i < argc; i++) {
		QString arg(argv[i]);
		if (arg == "-daemon") {
			bmain(argc, argv);
			exit(0);
		}
		if (arg == "-init") {	// spin us off so the startup can proceed
			if (fork())
				exit(0);
		}
	}
  }

  {
  	KInputApplication *app = new KInputApplication(argc, argv, "kcmlaptop");
  	app->setTitle(klocale->translate("Laptop Control Panels"));
 	 
  	if (app->runGUI())
    	return app->exec();
  	else
    	{
      	app->init();
      	return 0;
    	}
  }
}

