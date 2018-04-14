#include <view/settingview.h>


SettingView::SettingView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int SettingView::process(struct CookEvent& cookEvent)
{
	return 0;
}

