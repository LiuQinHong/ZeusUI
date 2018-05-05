#include <view/gameview.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <windowmanager.h>


GameView::GameView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int GameView::process(struct CookEvent& cookEvent)
{	
	

	pid_t g_Pid;
	int iStatus;

	g_Pid = fork();
	if (g_Pid == 0) {
		/* Child */		
		execlp("InfoNES", "InfoNES", getTargetFileName().c_str(), NULL);
		exit(0);
	}

	waitpid(g_Pid, &iStatus, 0);

	/* 目的是刷新一下屏幕 */
	WindowManager::getWindowManager()->addTask(getParentWindow(), REFRESH_WINDOW);	
	return 0;	
}





