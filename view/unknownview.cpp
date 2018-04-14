#include <view/unknownview.h>
#include <windowmanager.h>

UnknownView::UnknownView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int UnknownView::process(struct CookEvent& cookEvent)
{
	return 0;
}





