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
#include <ctime>

class Widget {
 public:
   Widget();
   ~Widget();
   virtual std::string getStatusbarOutput() const =0;
};

class WidgetManager {
 public:
   WidgetManager(const WidgetManager&) =delete;
   ~WidgetManager() {
      assert(mDisplay);
      XCloseDisplay(mDisplay);
      while (mWidgets.begin() != mWidgets.end()) {
         delete *mWidgets.begin();
      }
   }
   static WidgetManager& instance() {
      static WidgetManager mWidgetManager;
      return mWidgetManager;
   }
   void subscribe(const Widget* w) { 
      mWidgets.push_back(w); 
   }
   void unsubscribe(const Widget* w) {
      mWidgets.erase(std::find(mWidgets.begin(), mWidgets.end(), w)); 
   }
   void run(int refresh_rate=1) const {
      while (true) {
         updateStatusbar();
         sleep(refresh_rate);
      }
   }
 private:
   WidgetManager() {
      mDisplay = XOpenDisplay(0x0);
   }
   void updateStatusbar() const {
      static std::string statusbar("");
      statusbar.clear();
      for (constWidgetIter itr=mWidgets.begin(); itr!=mWidgets.end(); ++itr) {
         statusbar += " " + (*itr)->getStatusbarOutput();
      }
      statusbar.erase(std::remove(statusbar.begin(), statusbar.end(), '\n'), statusbar.end());
      assert(mDisplay);
      XStoreName(mDisplay, DefaultRootWindow(mDisplay), statusbar.c_str());
      XSync(mDisplay, 0);
   }

   using constWidgetIter = std::vector<const Widget*>::const_iterator;
   std::vector<const Widget*> mWidgets;
   Display* mDisplay;
};

Widget::Widget() {
   WidgetManager::instance().subscribe(this); 
}
Widget::~Widget() {
   WidgetManager::instance().unsubscribe(this); 
}

class TimeWidget: public Widget {
 public:
   TimeWidget() : Widget() {}
   virtual std::string getStatusbarOutput() const {
      time_t time_since_epoch;
      time(&time_since_epoch);
      return ctime(&time_since_epoch);
   }
};

int main(void){
   new TimeWidget();
   WidgetManager::instance().run();
   return 0;
}
