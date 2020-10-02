#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <qdir.h>
#include <kapp.h>
#include "kbattery.h"
#include "daemon.h"

BatteryDaemon::~BatteryDaemon()
{
}

BatteryDaemon::BatteryDaemon()
{
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
	
        testrunning();
}                                          

int BatteryDaemon::is_running()
{
	return(running);
}

void BatteryDaemon::testrunning()
{
	char	buff[20];
	int i, fd;

	running = 0;
	fd = ::open(lock.data(), O_RDONLY);
	if (fd >= 0) {
		i = ::read(fd, &buff[0], sizeof(buff)-1);
		buff[i] = 0;
		::close(fd);
		pid = ::atoi(buff);
		if (::kill(pid, 0) >= 0) {
			running = 1;
                    ::sleep(1);
                }
		::waitpid(0,0,WNOHANG);
	}
}

void BatteryDaemon::kill()
{
	testrunning();
	if (running)
		::kill(pid, 9);
	::unlink(lock.data());
	running = 0;
}

void BatteryDaemon::restart()
{
	kill();
	start();
}

void BatteryDaemon::start()
{
	pid = fork();
	if (pid) {
		running = 1;
		return;
	}
	::system("exec kcmlaptop -daemon&");
	::exit(0);
}
