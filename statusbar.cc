// dwm-statusbar - statusbar.cc
// author: johannst

#include <X11/Xlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <cassert>

class Widget {
 public:
   Widget();
   ~Widget();
   virtual std::string getStatusbarOutput() const =0;
};

class WidgetManager {
 public:
   WidgetManager() {
      mDisplay = XOpenDisplay(0x0);
   }
   ~WidgetManager() {
      assert(mDisplay);
      XCloseDisplay(mDisplay);
      while (mWidgets.begin() != mWidgets.end()) {
         delete *mWidgets.begin();
      }
   }
   void subscribe(const Widget* w) { 
      mWidgets.push_back(w); 
   }
   void unsubscribe(const Widget* w) {
      mWidgets.erase(std::find(mWidgets.begin(), mWidgets.end(), w)); 
   }
   void updateStatusbar() const {
      std::string statusbar("");
      for (tWidgetIter itr=mWidgets.begin(); itr!=mWidgets.end(); ++itr) {
         statusbar += " " + (*itr)->getStatusbarOutput();
      }
      assert(mDisplay);
      XStoreName(mDisplay, DefaultRootWindow(mDisplay), statusbar.c_str());
      XSync(mDisplay, 0);
   }
 private:
   typedef std::vector<const Widget*>::const_iterator tWidgetIter;
   std::vector<const Widget*> mWidgets;
   Display* mDisplay;
};
std::auto_ptr<WidgetManager> gWidgetManager(new WidgetManager);

Widget::Widget() {
   gWidgetManager->subscribe(this); 
}
Widget::~Widget() {
   gWidgetManager->unsubscribe(this); 
}

class TimeWidget: public Widget {
 public:
   TimeWidget() : Widget() {}
   virtual std::string getStatusbarOutput() const {
      return "TimeWdiget";
   }
};

int main(void){
   Widget* w = new TimeWidget();
   gWidgetManager->updateStatusbar();
}