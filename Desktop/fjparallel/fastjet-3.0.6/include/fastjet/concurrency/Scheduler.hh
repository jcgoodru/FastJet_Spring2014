
#include "Scheduler.cc" // NULL (as of now)
#include <vector>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
using namespace std ;



bool firstevent = true ; // sets itself to false at the end of every event

boost::mutex _mtx ;
boost::thread_group TGROUP ;




int flag = 0 ;
