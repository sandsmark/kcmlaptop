/*
 * keyboard.h
 *
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

#ifndef __KKEYBOARDCONFIG_H__
#define __KKEYBOARDCONFIG_H__

#include <qlabel.h>
#include <qradiobt.h>
#include <qbttngrp.h>
#include <qlineedit.h>
#include <qpushbt.h>
#include <kapp.h>

#include <kcontrol.h>


class PowerConfig : public KConfigWidget
{
  Q_OBJECT
public:
  PowerConfig( QWidget *parent=0, const char* name=0, bool init=FALSE );
  ~PowerConfig( );
  void  resizeEvent(QResizeEvent *e);
  void saveParams( void );

  void loadSettings();
  void applySettings();
  void defaultSettings();
  int getPower();
  int getNoPower();
  const int getNoEditWait() const;
  const int getEditWait() const;
      
private:
  void GetSettings( void );

  void setPower( int, int );


  QButtonGroup *nopowerBox;
  QRadioButton *nopowerStandby, *nopowerSuspend, *nopowerOff;
  QButtonGroup *powerBox;
  QRadioButton *powerStandby, *powerSuspend, *powerOff;
  QLineEdit *noeditwait;
  QLineEdit *editwait;
  QString edit_wait;
  QString noedit_wait;

  KConfig *config;
  int power, nopower, pm;
};

#endif

