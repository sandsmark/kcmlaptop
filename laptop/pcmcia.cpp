/*
 * pcmcia.cpp
 * Copyright (c) 1999 Paul Campbell paul@taniwha.com
 *
 * stolen from mouse.cpp
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

#include <iostream> 

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <qfileinf.h> 
#include <qstring.h>
#include <qmsgbox.h> 

#include <kmsgbox.h> 
#include <qlayout.h> 
#include "pcmcia.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "geom.h"

#include "version.h"

static bool GUI;

PcmciaConfig::~PcmciaConfig ()
{
  if (GUI)
    {
      delete label0_text;
      delete label1_text;
      delete label0;
      delete label1;
    }
}

PcmciaConfig::PcmciaConfig (QWidget * parent, const char *name, bool init)
  : KConfigWidget (parent, name)
{
  if (init)
    GUI = FALSE;
  else
    GUI = TRUE;

  if (GUI)
    {
      FILE *f = fopen("/var/run/stab", "r");
      if (f) {
	char tmp0[256], tmp1[256];
	char c, *cp;

	cp = tmp0;
	for (;;) {
		c = getc(f);
		if (c == EOF || c == '\n')
			break;
		if (c == ':') {
			while ((c = getc(f)) == ' ')
				;
			for (;;) {
				*cp++ = c;
				c = getc(f);
				if (c == EOF || c == '\n')
					break;
			}
			break;
		}
	}
	*cp = 0;

	cp = tmp1;
	for (;;) {
		c = getc(f);
		if (c == EOF || c == '\n')
			break;
		if (c == ':') {
			while ((c = getc(f)) == ' ')
				;
			for (;;) {
				*cp++ = c;
				c = getc(f);
				if (c == EOF || c == '\n')
					break;
			}
			break;
		}
	}
	*cp = 0;

	fclose(f);
      	label0 = new QLabel(klocale->translate("Card 0:"), this);
      	label1 = new QLabel(klocale->translate("Card 1:"), this);
      	label0_text = new QLabel(tmp0, this);
      	label1_text = new QLabel(tmp1, this);
      } else {
      	label0 = new QLabel(klocale->translate("No PCMCIA ccontroller detected"), this);
      	label1 = new QLabel(klocale->translate(""), this);
      	label0_text = new QLabel(klocale->translate(""), this);
      	label1_text = new QLabel(klocale->translate(""), this);
	
      }

	QVBoxLayout *top_layout = new QVBoxLayout(this, 15, 5);
	QGridLayout *top_grid = new QGridLayout(2, 2);
	top_layout->addLayout(top_grid);

	top_grid->setColStretch(0, 0);
	top_grid->setColStretch(1, 1);
	top_grid->addRowSpacing(0, 40);
	top_grid->addRowSpacing(1, 40);

	label0->setFixedSize(80, 24);
        top_grid->addWidget(label0, 0, 0);   
	label0_text->adjustSize();
        top_grid->addWidget(label0_text, 0, 1);   

	label1->setFixedSize(80, 24);
        top_grid->addWidget(label1, 1, 0);   
	label1_text->adjustSize();
        top_grid->addWidget(label1_text, 1, 1);   


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

  GetSettings();
}

void PcmciaConfig::resizeEvent(QResizeEvent *)
{
#ifdef NOTDEF
  int h = SPACE_YO;
  int w = 0;
  int center;
  
  w = max( label0->width(), label1->width() );

  label0->move(SPACE_XO, h);
  label0_text->move(4*SPACE_XO+w, h);
  h += (label0->height() + 3*SPACE_YI);


  label1->move(SPACE_XO, h);
  label1_text->move(4*SPACE_XO+w, h);
#endif
}

void PcmciaConfig::GetSettings( void )
{
  if (GUI)
    {
      ;
    }
}

void PcmciaConfig::saveParams( void )
{
  if (GUI)
    {
    }

  
}

void PcmciaConfig::loadSettings()
{
  GetSettings();
}

void PcmciaConfig::applySettings()
{
  saveParams();
}

void PcmciaConfig::defaultSettings()
{
}


#include "pcmcia.moc"
