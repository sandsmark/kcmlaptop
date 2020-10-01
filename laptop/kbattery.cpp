/*
 * kbattery.cpp
 * Copyright (C) 1998 Paul Campbell <paul@taniwha.com>
 *
 * from the KBiff source
 * Copyright (C) 1998 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the implementation of the main KBattery
 * widget
 *
 * $Id$
 */


#include <qdir.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <qmovie.h>
#include <qlist.h>
#include <qfileinf.h>
#include <qimage.h>

#include <kiconloader.h>
#include <kprocess.h>
#include <kwm.h>
#include <qtooltip.h>

#include "kbattery.h"
#include "notify.h"
#include "power.h"
#include "warning.h"
#include "battery.h"
#include "apm.h"

KBattery::KBattery(PowerConfig *p, BatteryWarning *w, BatteryWarning *c, BatteryConfig *b, QWidget *parent)
	: QLabel(parent)
{
	current_code = 0;
	pm = 0;
	backoff = 0;
	power = p;
	warning = w;
	critical = c;
	battery = b;
	triggered[0] = 0;
	triggered[1] = 0;
	setAutoResize(true);
	setMargin(0);
	setAlignment(AlignLeft | AlignTop);

	// Init the audio server.  serverStatus return 0 when it is ok
	hasAudio = (audioServer.serverStatus() == 0) ? true : false;

	// enable the session management stuff
	//connect(kapp, SIGNAL(saveYourself()), this, SLOT(saveYourself()));
          disconnect(this);
          connect(this, SIGNAL(signal_checkBattery()), SLOT(checkBatteryNow()));

	reset();
	last_time = time(0);
	if (power->getPower() || power->getNoPower()) {
		if (powered) {
			power_time = time(0)+60*power->getEditWait();
		} else {
			power_time = time(0)+60*power->getNoEditWait();
		}	
		open_interrupts();
		timer =  new QTimer( this );
		connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );
		timer->start( 2*1000, TRUE );                 // 1 seconds single-shot
	} else {
		timer = 0;
	}
  	config = kapp->getConfig();

}

KBattery::~KBattery()
{
}

void KBattery::timerDone()
{
	unsigned long t = time(0);
	if (t >= (last_time+120)) {	// time suddenly jumped - we just powered up?
		backoff = 0;
		if (powered) {
			power_time = t+60*power->getEditWait();
		} else {
			power_time = t+60*power->getNoEditWait();
		}	
	} else
	if (backoff) {
		if (t >= power_time) {
			backoff = 0;
			if (powered) {
				power_time = t+60*power->getEditWait();
			} else {
				power_time = t+60*power->getNoEditWait();
			}	
		}
	} else
	if (poll_interrupts()) {
		if (powered) {
			power_time = t+60*power->getEditWait();
		} else {
			power_time = t+60*power->getNoEditWait();
		}	
	} else 
	if (t >= power_time) {
		int val;

		if (powered) {
			val = power->getPower();
		} else {
			val = power->getNoPower();
		}	
		switch (val) {
		case 1:
			invokeStandby();
			break;
		case 2:
			invokeSuspend();
			break;
		}
		backoff = 1;
		power_time = t+60;		// wait to give us time to get in and out of suspend prior to 
						// suspend
	}
	last_time = t;
	timer->start( 2*1000, TRUE );           // 1 seconds single-shot
}

//
//	boy this is tacky - totally non-portable - but then
//	this is a Linux specific panel - I did this this way
//	mainly 'cause I don't know if X can handle 2 things trying
//	to be a sceensaver at the same time (one real one and 
//	this guy thinking about powering down) and I want both to work
//	together
//
int KBattery::poll_interrupts()
{
	static int mouse_count = 0, key_count = 0;

	int m=0, k = 0;
	int v;
	char	name[256];
	char *cp, *cp2;
	int *vp;

	if (procint == 0)
		return(0);

	::rewind(procint);
	for (;;) {
		if (::fgets(name, sizeof(name), procint) == 0)
			break;
		vp = 0;
		if (strstr(name, "Mouse") || strstr(name, "mouse")) {
			vp = &m;
		} else
		if (strstr(name, "Keyboard") || strstr(name, "keyboard"))
			vp = &k;
		if (vp == 0)
			continue;
		v = 0;
		for (cp = name;*cp;cp++) {
			if (*cp != ':')
				continue;
			cp++;
			for (;;) {
				for(;;cp++) {
					if (*cp != ' ' && *cp != '\t')
						break;
				}
				if (*cp < '0' || *cp > '9')
					break;
				cp2 = cp;
				while (*cp >= '0' && *cp <= '9')
					cp++;
				
				*cp++ = 0;
				v += atoi(cp2);	
			}
			break;
		}
		if (v > *vp)
			*vp = v;
	}
	v = k != key_count || m != mouse_count;
	key_count = k;
	mouse_count = m;
	return(v);
}

void KBattery::open_interrupts()
{
	procint = fopen("/proc/interrupts", "r");
	(void)poll_interrupts();
}

void KBattery::processSetup()
{
	// General settings
	noBatteryIcon  = battery->getButtonNoBattery();
	chargeIcon     = battery->getButtonCharge();
	noChargeIcon    = battery->getButtonNoCharge();
	poll           = battery->getPoll();
	docked 	       = battery->getDock();
	enabled 	      = battery->getEnabled();
	if(!enabled)
		docked = 0;

	systemBeep[0]     = warning->getSystemBeep();
	runCommand[0]     = warning->getRunCommand();
	runCommandPath[0] = warning->getRunCommandPath();
	playSound[0]      = warning->getPlaySound();
	playSoundPath[0]  = warning->getPlaySoundPath();
	notify[0]         = warning->getNotify();
	low[0]            = warning->getLowValue();
	do_suspend[0]     = warning->getSuspend();
	do_standby[0]     = warning->getStandby();
	have_time         = warning->getHaveTime();

	systemBeep[1]     = critical->getSystemBeep();
	runCommand[1]     = critical->getRunCommand();
	runCommandPath[1] = critical->getRunCommandPath();
	playSound[1]      = critical->getPlaySound();
	playSoundPath[1]  = critical->getPlaySoundPath();
	notify[1]         = critical->getNotify();
	low[1]            = critical->getLowValue();
	do_suspend[1]     = critical->getSuspend();
	do_standby[1]     = critical->getStandby();

	// change the dock state if necessary
	if (docked)
		dock();

	start();

}

const bool KBattery::isDocked() const
{
	return docked;
}

//void KBattery::readSessionConfig()
//{
//	docked = 0;
//
//	processSetup();
//}

///////////////////////////////////////////////////////////////////////////
// Protected Virtuals
///////////////////////////////////////////////////////////////////////////
void KBattery::mousePressEvent(QMouseEvent *event)
{
	// check if this is a right click
	if(event->button() == RightButton)
	{
		// popup the context menu
		popupMenu(1);
	} else {
		popupMenu(0);
	}
}

///////////////////////////////////////////////////////////////////////////
// Protected Slots
///////////////////////////////////////////////////////////////////////////
//void KBattery::saveYourself()
//{
//		KConfig *config = kapp->getSessionConfig();
//		config->setGroup("KBattery");
//
//		config->writeEntry("IsDocked", docked);
//
//		config->sync();
//}

void KBattery::invokeSuspend()
{
	::system("/usr/bin/apm --suspend");
}

void KBattery::invokeStandby()
{
	::system("/usr/bin/apm --standby");
}

bool KBattery::getExists()
{
	return(exists);
}

bool KBattery::getPowered()
{
	return(powered);
}

void KBattery::displayPixmap()
{
	int new_code;

	if (!exists)
		new_code = 1;
	else if (!powered)
		new_code = 2;
	else
		new_code = 3;

	if (have_time == 2 && exists && !powered) {		// in some circumstances 
		config->setGroup("BatteryLow");			// we can;t figure this out 'till
		have_time = (val < 0 ? 0 : 1);			// the battery is not charging
		config->writeEntry("HaveTime", have_time);
		config->sync();
	}
	
	if (pm == 0 || current_code != new_code) {
		current_code = new_code;

		// we will try to deduce the pixmap (or gif) name now.  it will
		// vary depending on the dock and power
		QString pixmap_name, mini_pixmap_name;
	
		if (!exists)
			pixmap_name = noBatteryIcon;
		else if (!powered)
			pixmap_name = noChargeIcon;
		else
			pixmap_name = chargeIcon;
		mini_pixmap_name = "mini/" + pixmap_name;
	
		// Get a list of all the pixmap paths.  This is needed since the
		// icon loader only returns the name of the pixmap -- NOT it's path!
		QStrList *dir_list = kapp->getIconLoader()->getDirList();
		QFileInfo file, mini_file;
		for (unsigned int i = 0; i < dir_list->count(); i++) {
			QString here = dir_list->at(i);
	
			// check if this file exists.  If so, we have our path
			file.setFile(here + '/' + pixmap_name);
			mini_file.setFile(here + '/' + mini_pixmap_name);
			if (docked && !mini_file.exists())
				mini_file.setFile(here + "/mini-"+pixmap_name);
	
			// if we are docked, check if the mini pixmap exists FIRST.
			// if it does, we go with it.  if not, we look for the
			// the regular size one
			if (docked && mini_file.exists())
			{
				file = mini_file;
				break;
			}
	
			if (file.exists())
				break;
		}
		if (pm)
			delete pm;
		pm = new QPixmap(file.absFilePath());
	}

	// at this point, we have the file to display.  so display it
	QImage image = pm->convertToImage();

	int w = image.width();
	int h = image.height();
	int count = 0;
	QRgb rgb;
	int x, y;
	for (x = 0; x < w; x++)
	for (y = 0; y < h; y++) {
		rgb = image.pixel(x, y);
		if (qRed(rgb) == 0xff &&
		    qGreen(rgb) == 0xff &&
		    qBlue(rgb) == 0xff)
			count++;
	}
	int c = (count*val)/100;
	if (val == 100) {
		c = count;
	} else
	if (val != 100 && c == count)
		c = count-1;

	
	if (c) {
		uint ui;
		QRgb blue = qRgb(0x00,0x00,0xff);

		if (image.depth() <= 8) {
			ui = image.numColors();		// this fix thanks to Sven Krumpke
			image.setNumColors(ui+1);
			image.setColor(ui, blue);
		} else {
			ui = 0xff000000|blue;
		}
	
		for (y = h-1; y >= 0; y--) 
		for (x = 0; x < w; x++) {
			rgb = image.pixel(x, y);
			if (qRed(rgb) == 0xff &&
		    	    qGreen(rgb) == 0xff &&
		    	    qBlue(rgb) == 0xff) {
				image.setPixel(x, y, ui);
				c--;
				if (c <= 0)
					goto quit;
			}
		}
	}
quit:

	QPixmap q;
	q.convertFromImage(image);
	setPixmap(q);
	adjustSize();
	if (left >= 0) {
		if (!triggered[0]) {
			if (exists && !powered && left <= low[0]) {
				triggered[0] = 1;
				haveBatteryLow(0, left, 0);
			}
		} else {	
			if (!triggered[1]) {
				if (exists && !powered && left <= low[1]) {
					triggered[1] = 1;
					haveBatteryLow(1, left, 0);
				}
			}
			if (left > (low[1]+1))
				triggered[1] = 0;
			if (left > low[0])
				triggered[0] = 0;
		}
	} else
	if (have_time != 1) {
		if (!triggered[0]) {
			if (exists && !powered && val <= low[0]) {
				triggered[0] = 1;
				haveBatteryLow(0, val, 1);
			}
		} else {	
			if (!triggered[1]) {
				if (exists && !powered && val <= low[1]) {
					triggered[1] = 1;
					haveBatteryLow(1, val, 1);
				}
			}
			if (val > (low[1]+1))
				triggered[1] = 0;
			if (val > low[0])
				triggered[0] = 0;
		}
	}
	char tmp[1024];
	if (!exists) {
		sprintf(tmp, i18n("APM not available"));
	} else
	if (powered) {
		if (val == 100) {
			sprintf(tmp, "%s - %s", i18n("Plugged in"), i18n("fully charged"));
		} else {
			if (left >= 0) {
				sprintf(tmp, "%s - %d%% %s (%d:%02d %s)", i18n("Plugged in"), val, i18n("charged"), left/60, left%60, i18n("minutes"));
			} else {
				sprintf(tmp, "%s - %d%% %s", i18n("Plugged in"), val, i18n("charged"));
			}
		}
	} else {
		if (left >= 0) {
			sprintf(tmp, "%s - %d%% %s (%d:%02d %s)", i18n("Running on batteries"), val, i18n("charged"), left/60, left%60, i18n("minutes"));
		} else {
			sprintf(tmp, "%s - %d%% %s", i18n("Running on batteries"), val, i18n("charged"));
		}
	}
	QToolTip::add(this, tmp); 
}

void KBattery::haveBatteryLow(int t, const int num, const int type)
{
	displayPixmap();

	// beep if we are allowed to
	if (systemBeep[t]) {
		kapp->beep();
	}

	// run a command if we have to
	if (runCommand[t]) {
		// make sure the command exists
		if (!runCommandPath[t].isEmpty()) {
			KProcess command;
			command << runCommandPath[t];
			command.start(KProcess::DontCare);
		}
	}

	if (do_suspend[t])
		invokeSuspend();
	if (do_standby[t])
		invokeStandby();

	// play a sound if we have to
	if (playSound[t] && hasAudio) {
		// make sure something is specified
		if (!playSoundPath[t].isEmpty()) {
			audioServer.play(playSoundPath[t]);
			audioServer.sync();
		}
	}

	// notify if we must
	if (notify[t]) {
		KBatteryNotify notify_dlg(num, type);
		notify_dlg.exec();
	}
}

void KBattery::noop()
{
}

void KBattery::do_dock()
{
	docked = !docked;
	dock();
}

void KBattery::dock()
{
	// destroy the old window
	if (this->isVisible()) {
		this->hide();
		this->destroy(true, true);
		this->create(0, true, false);
		kapp->setMainWidget(this);

		// we don't want a "real" top widget if we are _going_ to
		// be docked.
		if (!docked)
			kapp->setTopWidget(this);
		else
			kapp->setTopWidget(new QWidget);
	}

	if (docked) {

		// enable docking
		KWM::setDockWindow(this->winId());
	}

	// (un)dock it!
	if (enabled) {
		this->show();
	} else {
		this->hide();
	}
	displayPixmap();
}

void KBattery::setup()
{
	::system("kcmlaptop");
}

void KBattery::quit()
{
	enabled = 0;
	if (docked) {
		docked = 0;
		dock();
	}
	this->hide();
}

void KBattery::checkBatteryNow()
{
	apm_info x = {{10*0},0,0,0,0,0,0,0,0,0};
	if (apm_read(&x) || (x.apm_flags&0x20)) {
		powered = 0;
		exists=0;
		val=0;
		left = 0;
	} else {
		powered = x.ac_line_status&1;
		val = x.battery_percentage;
		left = x.battery_time;
		exists=1;
	}
	if (timer && oldpowered != powered) {
		if (powered) {
			power_time = time(0)+60*power->getEditWait();
		} else {
			power_time = time(0)+60*power->getNoEditWait();
		}	
	}
	changed =  oldpowered != powered||oldexists != exists||oldval != val;
	oldpowered = powered;
	oldexists = exists;
	oldval = val;
	if (changed)
		displayPixmap();
}

void KBattery::start()
{
	checkBatteryNow();
	displayPixmap();
	oldTimer = startTimer(poll * 1000);
}

void KBattery::setPollInterval(const int interval)
{
        poll = interval;

        // Kill any old timers that may be running
        if (oldTimer > 0)
        {
                killTimer(oldTimer);

                // Start a new timer will the specified time
                oldTimer = startTimer(interval * 1000);

                emit(signal_checkBattery());
        }
}

void KBattery::timerEvent(QTimerEvent *)
{
        emit(signal_checkBattery());
}


///////////////////////////////////////////////////////////////////////////
// Protected Functions
///////////////////////////////////////////////////////////////////////////
void KBattery::popupMenu(int type)
{
	QPopupMenu *popup = new QPopupMenu(0, "popup");
	char	tmp[1024];
	struct stat s;

	if (type) {
		if (docked)
			popup->insertItem(i18n("&UnDock"), this, SLOT(do_dock()));
		else
			popup->insertItem(i18n("&Dock"), this, SLOT(do_dock()));



		if (!stat("/usr/bin/apm", &s) && (getuid() == 0 || s.st_mode&S_ISUID)) {
			popup->insertItem(i18n("Setup..."), this, SLOT(setup()));
			popup->insertSeparator();
			popup->insertItem(i18n("Standby..."), this, SLOT(invokeStandby()));
			popup->insertItem(i18n("&Suspend..."), this, SLOT(invokeSuspend()));
			popup->insertSeparator();
		} else {
			popup->insertItem(i18n("&Setup..."), this, SLOT(setup()));
			popup->insertSeparator();
		}
		popup->insertSeparator();
		popup->insertItem(i18n("&Quit..."), this, SLOT(quit()));

	} else 
	if (!exists) {
		popup->insertItem(i18n("APM Manager Not Found"), this, SLOT(noop()));
	} else {
		if (left < 0) {	// buggy BIOS
			sprintf(tmp, "%%%d %s", val, i18n("charged"));
		} else {
			sprintf(tmp, "%d:%02d %s", left/60, left%60, i18n("minutes left"));
		}
		popup->insertItem(i18n(tmp), this, SLOT(noop()));
		popup->setItemEnabled(0, 0);
		popup->insertSeparator();
		if (powered) {
			popup->insertItem(i18n("Charging"), this, SLOT(noop()));
		} else {
			popup->insertItem(i18n("Not Charging"), this, SLOT(noop()));
		}
		popup->setItemEnabled(1, 0);
	}

	popup->popup(QCursor::pos());
}

void KBattery::reset()
{
	// reset all the member variables
	systemBeep[0]     = true;
	runCommand[0]     = false;
	runCommandPath[0] = "";
	playSound[0]      = false;
	playSoundPath[0]  = "";
	notify[0]         = true;
	do_suspend[0]     = false;
	do_standby[0]     = false;

	systemBeep[1]     = true;
	runCommand[1]     = false;
	runCommandPath[1] = "";
	playSound[1]      = false;
	playSoundPath[1]  = "";
	notify[1]         = true;
	do_suspend[1]     = true;
	do_standby[1]     = false;

	noBatteryIcon  = "laptop_nobattery.xpm";
	chargeIcon = "laptop_charge.xpm";
	noChargeIcon = "laptop_nocharge.xpm";

	docked    = true;
	poll	  = 20;
	low[0]	  = 15;
	low[1]	  = 5;
	processSetup();
	start();
}

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <kapp.h>

KApplication *this_app;
KBattery *kbattery;


int bmain(int argc, char *argv[])
{
	int fd, i;
        QString lock_link, pid, lock, ldir;
        QDir    dir;
        ldir = KApplication::localkdedir();

        //
        //      first make a ~/.kde/share/apps/"app" directory if it doesn't exist
        //
        ldir += "/share/apps";
        dir.setPath(ldir.data());
        if(!dir.exists()){
                dir.mkdir(ldir.data());
                chmod(ldir.data(), S_IRWXU);
        }

        ldir += "/kcmlaptop";
        dir.setPath(ldir.data());
        if(!dir.exists()){
                dir.mkdir(ldir.data());
                chmod(ldir.data(), S_IRWXU);
        }
        ldir += "/";

        lock = ldir + "daemon_lock";
	
	struct stat sbuff;

        lock_link.sprintf("%sdaemon_lock_%d", ldir.data(), getpid());
        pid.sprintf("%d", getpid());
	(void)::unlink(lock_link.data());       // remove any old one
        fd = ::open(lock_link.data(), O_CREAT|O_RDWR, 0777);
        if (fd < 0) {

                (void)::exit(1);
        }
        (void)::write(fd, pid.data(), pid.length());
	(void)::close(fd);
	for (i = 0; i < 10; i++) {
                (void)::link(lock_link.data(), lock.data());    // atomic

                //
                //      we got the lock if the link succeeded and the nlink is 2
                //

                if (::stat(lock_link.data(), &sbuff) >= 0 && sbuff.st_nlink == 2) {     // I got the lock -- hooray!
			(void)::unlink(lock_link.data());       // remove any old one

			KApplication app(argc, argv, "kcmlaptop");

			this_app = &app;
  			PowerConfig *power = new PowerConfig(0, 0, TRUE);
  			power->loadSettings();

  			BatteryWarning *warning = new BatteryWarning(0, 0, 0, TRUE);
  			warning->loadSettings();

  			BatteryWarning *critical = new BatteryWarning(1, 0, 0, TRUE);
  			critical->loadSettings();

  			BatteryConfig *battery = new BatteryConfig(0, 0, TRUE);
  			battery->loadSettings();

			kbattery = new KBattery(power, warning, critical, battery);

			kbattery->processSetup();
			app.setMainWidget(kbattery);
			


			(void)app.exec();
			// never returns
			(void)::exit(0);
		}
		sleep(1);
		fd = ::open(lock.data(), O_RDONLY);
		if (fd >= 0) {
			char buff[20];
			int j = ::read(fd, &buff[0], sizeof(buff)-1);
			buff[j] = 0;
			::close(fd);
			j = ::atoi(buff);
			if (j > 1)
				::kill(j, 0);
			::unlink(lock.data());
		}
		::sleep(1);
		::waitpid(0,0,WNOHANG);
	}
	::exit(0);
}

#include "kbattery.moc"
