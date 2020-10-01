/*
 * setupdlg.h
 * Copyright (C) 1998 Paul Campbell <paul@taniwha.com>
 *
 * from the KBiff source
 * Copyright (C) 1998 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the setup dialog and related widgets
 * for KBattery.  All user configuration is done here.
 *
 * $Id$
 */
#ifndef WSETUPDLG_H
#define WSETUPDLG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif //HAVE_CONFIG_H

#include <qglobal.h>
#if QT_VERSION >= 140
#include <qlistview.h>
#else
#include <ktreeview.h>
#endif

#include <qwidget.h>
#include <qdialog.h>

class QLineEdit;
class QCheckBox;
class QPushButton;
class QComboBox;

#include <kiconloaderdialog.h>
#include <kcontrol.h>
class BatteryWarning : public KConfigWidget
{
	Q_OBJECT
public:
	BatteryWarning(int t, QWidget *parent=0, const char* name=0, bool init=FALSE );  
	virtual ~BatteryWarning();

	const unsigned int getLowValue() const;
	const bool getRunCommand() const;
	const char* getRunCommandPath() const;
	const bool getPlaySound() const;
	const char* getPlaySoundPath() const;
	const bool getSystemBeep() const;
	const bool getNotify() const;
	const bool getSuspend() const;
	const bool getStandby() const;
	const int getHaveTime() const;

  void  resizeEvent(QResizeEvent *e);
  void saveParams( void );

  void loadSettings();
  void applySettings();
  void defaultSettings();
  void GetSettings(int x);

public slots:
	void enableRunCommand(bool);
	void enablePlaySound(bool);
	void browseRunCommand();
	void browsePlaySound();

private:
  KConfig *config;

	QLineEdit *editRunCommand;
	QLineEdit *editPlaySound;
	QLineEdit* editLow;

	QCheckBox *checkRunCommand;
	QCheckBox *checkPlaySound;
	QCheckBox *checkBeep;
	QCheckBox *checkNotify;
	QCheckBox *checkSuspend;
	QCheckBox *checkStandby;

	QPushButton *buttonBrowsePlaySound;
	QPushButton *buttonBrowseRunCommand;

	bool	apm, runcommand, playsound, beep, notify, do_suspend, do_standby;
	QString	runcommand_val, low_val, crit_val, sound_val;
	int		have_time, type;
};

#endif // SETUPDLG_H
