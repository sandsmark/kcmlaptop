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
#ifndef SETUPDLG_H
#define SETUPDLG_H

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


class BatteryConfig : public KConfigWidget
{
  Q_OBJECT
public:
  BatteryConfig( QWidget *parent=0, const char* name=0, bool init=FALSE );
  ~BatteryConfig( );
  void  resizeEvent(QResizeEvent *e);
  void saveParams( void );

  void loadSettings();
  void applySettings();
  void defaultSettings();

	const char* getButtonCharge() const;
	const char* getButtonNoCharge() const;
	const char* getButtonNoBattery() const;
	const int   getPoll() const;
	const bool  getDock() const;
	const bool  getEnabled() const;
private:
  void GetSettings( void );

private:
  KConfig *config;

	QLineEdit* editPoll;
	QCheckBox* checkDock;
	QCheckBox* runMonitor;
	bool		enablemonitor;

	KIconLoaderButton *buttonNoBattery;
	KIconLoaderButton *buttonNoCharge;
	KIconLoaderButton *buttonCharge;
	QString nobattery, nochargebattery, chargebattery; 
	bool	docked, pm;
	QString	poll_time;
};

class KBatteryNewDlg : public QDialog
{
	Q_OBJECT
public:
	KBatteryNewDlg(QWidget* parent = 0, const char* name = 0);

	const char* getName() const
		{ return editName->text(); }

private:
	QLineEdit *editName;
};

#endif // SETUPDLG_H
