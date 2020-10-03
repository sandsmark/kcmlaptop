/*
 * kbattery.h
 * Copyright (C) 1998 Paul Campbell <paul@taniwha.com>
 *
 * from the KBiff source
 * Copyright (C) 1998 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the declaration of the main KBattery
 * widget.
 *
 * $Id$
 */
#ifndef KBATTERY_H 
#define KBATTERY_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qlist.h>

#include <kapp.h>
#include <stdio.h>

// mediatool.h is needed by kaudio.h
extern "C" {
#include <mediatool.h>
} 
#include <kaudio.h>

#include <qlabel.h>
#include "battery.h"
#include "power.h"
#include "warning.h"

class KBattery : public QLabel
{
	Q_OBJECT
public:
	KBattery(PowerConfig *p, BatteryWarning *w, BatteryWarning *c, BatteryConfig *b, QWidget *parent = 0);
	virtual ~KBattery();

	const bool isDocked() const;
	bool getExists();
	bool getPowered();

	void processSetup();

	void setPollInterval(const int poll=60);

protected:
	void mousePressEvent(QMouseEvent *);

signals:
    	void signal_checkBattery();

protected:
	void popupMenu(int type);
	void reset();
	void timerEvent(QTimerEvent *);

protected slots:
	void invokeStandby();
	void invokeSuspend();
	void displayPixmap();
	void do_dock();
	void quit();
	void noop();
	void haveBatteryLow(const int t, const int num, const int type);
	void setup();
	void checkBatteryNow();
	void start();
	void timerDone();

protected:

	void dock();
	// Capability
	bool    hasAudio;
	KAudio  audioServer;
	
	// General settings

	int	val;
	int	exists;
	int	powered;
	int	left;
	int	triggered[2];

	int	oldval, oldexists, oldpowered;

	int	changed;

	// 
	bool    systemBeep[2];
	bool    runCommand[2];
	QString runCommandPath[2];
	bool    playSound[2];
	QString playSoundPath[2];
	bool    notify[2];
	bool    do_suspend[2];
	bool    do_standby[2];
	int	poll;
	int	low[2];
	int     oldTimer;
	QTimer  *timer;

	bool    docked, enabled, backoff;
	unsigned long	power_time;
	unsigned long	last_time;
        int idle_seconds;
	int	poll_idle();

	QString noBatteryIcon;
	QString chargeIcon;
	QString noChargeIcon;

  	PowerConfig *power;
  	BatteryWarning *warning;
  	BatteryWarning *critical;
  	BatteryConfig *battery;

	QPixmap	*pm;
	int	current_code;
	int	have_time;
  	KConfig *config;
};

#endif // KBATTERY_H 
