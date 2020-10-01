#include <qstring.h>
class BatteryDaemon 
{
public:
  	BatteryDaemon();
  	~BatteryDaemon( );

   	int	is_running();
   	void	kill();
	void	restart();
	void	start();
	void	changed();
private:
	void	internal_daemon();
	void	testrunning();
	void	getlock();
	int 	running;
	int	pid;
	QString	lock;
	QString	ldir;
};

