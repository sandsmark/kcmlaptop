/*
 * notify.cpp
 * Copyright (C) 1998 Paul Campbell <paul@taniwha.com>
 *
 * from the KBiff source
 * Copyright (C) 1998 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the implementation of the KBatteryNotify
 * widget
 *
 * $Id$
 */
#include "notify.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpushbt.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <kiconloaderdialog.h>
#include <kapp.h>

KBatteryNotify::KBatteryNotify(const int num_new, const int type)
	: QDialog(0, 0, true, 0)
{
	struct stat s;

	setIcon(ICON("laptop_battery.xpm"));
	setCaption(i18n("Battery Power is running out!"));

	resize(0, 0);

	QVBoxLayout *layout = new QVBoxLayout(this, 12);

	QHBoxLayout *upper_layout = new QHBoxLayout();
	layout->addLayout(upper_layout);

	QLabel *pixmap = new QLabel(this);
	pixmap->setPixmap(ICON("battery.xpm"));
	pixmap->setFixedSize(pixmap->sizeHint());
	upper_layout->addWidget(pixmap);

	QVBoxLayout *power_layout = new QVBoxLayout();
	upper_layout->addLayout(power_layout);

	QLabel *congrats = new QLabel(i18n("Battery Power is running out!"), this);
	QFont the_font(congrats->font());
	the_font.setBold(true);
	congrats->setFont(the_font);
	congrats->setMinimumSize(congrats->sizeHint());
	power_layout->addWidget(congrats);

	QString msg;
	if (type) {
		msg.sprintf(i18n("Charge Left: %%%d"), num_new);
	} else {
		msg.sprintf(i18n("Minutes Left: %d"), num_new);
	}
	QLabel *how_many = new QLabel(msg, this);
	how_many->setMinimumSize(how_many->sizeHint());
	power_layout->addWidget(how_many);

	QPushButton *ok = new QPushButton(i18n("Continue"), this);
	ok->setDefault(true);
	ok->setFixedSize(ok->sizeHint());
	connect(ok, SIGNAL(clicked()), SLOT(accept()));

	QHBoxLayout *button_layout = new QHBoxLayout();
	layout->addLayout(button_layout);

	button_layout->addStretch(1);
	button_layout->addWidget(ok);
	button_layout->addStretch(1);
	
        QPushButton *susp = new QPushButton(i18n("Suspend Now"), this);
        susp->setFixedSize(susp->sizeHint());
        connect(susp, SIGNAL(clicked()), SLOT(dosusp()));

        button_layout->addWidget(susp);
        button_layout->addStretch(1);

	layout->activate();
}

void KBatteryNotify::dosusp()
{
	system("exec /usr/bin/pm --suspend");
	accept();
}

KBatteryNotify::~KBatteryNotify()
{
}
#include "notify.moc"
