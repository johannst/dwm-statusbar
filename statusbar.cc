// dwm-statusbar - statusbar.cc
// author: johannst

#include <X11/Xlib.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <ctime>

class Widget {
 public:
   Widget();
   Widget(const Widget&) =delete;
   Widget& operator=(const Widget&) =delete;
   virtual ~Widget();
   virtual std::string getStatusbarOutput() const =0;
};

class WidgetManager {
 public:
   WidgetManager(const WidgetManager&) =delete;
   WidgetManager& operator=(const WidgetManager&) =delete;
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
         statusbar += " | " + (*itr)->getStatusbarOutput();
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

class CpuUtilWidget: Widget {
 public:
   CpuUtilWidget() : Widget(), mLastIdleTime(0), mLastTotalTime(0) {}
   virtual std::string getStatusbarOutput() const {
      return std::to_string(calculateCpuUtilization()) + "%";
   }
 private:
   mutable size_t mLastIdleTime;
   mutable size_t mLastTotalTime;

   size_t calculateCpuUtilization() const {
      size_t idle_time=0;
      size_t total_time=0;
      parseCpuTimes(idle_time, total_time);

      const float idle_time_delta = idle_time - mLastIdleTime;
      const float total_time_delta = total_time - mLastTotalTime;
      const size_t utilization = 100.0 * (1.0 - idle_time_delta / total_time_delta);

      mLastIdleTime = idle_time;
      mLastTotalTime = total_time;

      return utilization;
   }

   void parseCpuTimes(size_t& idle_time, size_t& total_time) const {
      std::ifstream cpu_time_file("/proc/stat");
      cpu_time_file.ignore(5, ' ');

      std::vector<size_t> parsed_cpu_times;
      for (size_t time; cpu_time_file >> time; parsed_cpu_times.push_back(time));
      cpu_time_file.close();
      assert(parsed_cpu_times.size()>4);

      idle_time = parsed_cpu_times[3];
      total_time = std::accumulate(parsed_cpu_times.begin(), parsed_cpu_times.end(), 0);
   }
};

int main(void) {
   new CpuUtilWidget();
   new TimeWidget();
   WidgetManager::instance().run();
   return 0;
}
