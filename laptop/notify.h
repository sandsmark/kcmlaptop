/*
 * notify.h
 * Copyright (C) 1998 Paul Campbell <paul@taniwha.com>
 *
 * from the KBiff source
 * Copyright (C) 1998 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the declaration of the KBatteryNotify
 * widget.
 *
 * $Id$
 */
#ifndef KBATTERYNOTIFY_H 
#define KBATTERYNOTIFY_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qdialog.h>

class KBatteryNotify : public QDialog
{
	Q_OBJECT
public:
	KBatteryNotify(const int num_new, const int type);
	virtual ~KBatteryNotify();
protected slots:
	void dosusp();
};

#endif // KBATTERYNOTIFY_H
