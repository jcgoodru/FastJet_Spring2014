/*
  The point of this exercise is to show how a scheduler could possibly work.

  This program assumes that the program using this scheduling system:
    (1)  Has a main flow of control (can be called multiple times)
    (2)  Has threads that execute a process (can be called multiple times)
    (3)  The threads only operate when called by the main, and can operate
         infinitely many times

   FLOW OF
   CONTROL


   [[MAIN]]

   (start)
      |
      |          [[THR1]]  [[THR2]]  [[THR3]]
      |
(calls threads)->---v---->----v---->----v
                    v         v         v
                 (start)   (start)   (start)
                    |         |         |
                    |         |         |
                    |         |         |
                 (done)    (done)    (done)
                   ((wait for ALL done))
                              v
                              v
      v----<----<----<----<----
(wait for ALL)
      |
      |
      |


***The threads are designed to loop to the top, just in case:
    (1)  main needs to run again, or
    (2)  this specific process needs to run again
*/
#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#define open true
#define closed false
#define NUMBER_OF_THREADS 50
#define NUMBER_OF_EVENTS 6

using namespace std ;
using namespace boost ;



//////-----#include<THESE_OBJECTS.hh>----------------------
    bool firstevent = true ;  //changes after the first event

    bool TOP = closed ;       //TOP regulates the flow of the thread
    bool BOTTOM = closed ;    //BOTTOM also regulates thread flow

    mutex _mtx ;        //the only lock, used to grant access to the flag
    thread_group TGROUP ;   //the threads are stored here

    int flag = NUMBER_OF_THREADS ;  //when a thread is complete, it decrements flag.
                                    //(flag=0) signifies completion of the handshake
//////------------------------------------------------------

    int eventnumber = 0 ; //ignore this...

//////-----function definitions-----------------------------

//USED BY THE THREADS:
     void handshake_algorithm() ;  //The handshake portion of CS_TiledN2, with synchronization features added
     void STAGE_TWO() ;  //just the raw handshake algorithm, no synchronization
     void TELL_MAIN_IM_READY() ;  //decrement flag by 1 (meaning, one thread just finished)

//USED BY THE SCHEDULER:
     void WAIT_FOR_THREADS() ;     //wait for threaads to finish before proceeding
     void STAGE_ONE() ;     //pre-handshake FastJet operations
     void STAGE_THREE() ;   //post-handshake FastJet operations
     void RESET_FLAG() ;  //reset the flag, so that it can be used during the next run
     void CREATE_THREADS( void (*handshake_algorithm)() ) ;

//////------------------------------------------------------








int main()  //THIS IS EQUIVALENT TO ClusterSequence::Tiled_N2.cc
{
  while( eventnumber < NUMBER_OF_EVENTS )
  {
//STAGE ONE//
   //INITIALLY, THE MAIN() IS IN CONTROL.  THE MAIN WILL 
   //INITIALIZE THREADS, WHICH ARE DESIGNED TO WAIT FOR
   //A SIGNAL TO START
    eventnumber++ ;  //just to keep track of which event this is
    if(firstevent) //creates reusable threads 
      { CREATE_THREADS( handshake_algorithm );}
    WAIT_FOR_THREADS() ;  //wait for all threads to instantiate (or, reset if Event2 or later)
    BOTTOM = closed ;  //close the bottom of the threads
    RESET_FLAG() ;  //reset the flag, because we're about to start the threads
    STAGE_ONE() ;  //start the pre-handshake FastJet computations

//STAGE TWO//
   //AT THIS POINT, THE MAIN WILL STOP.  THE THREADS WILL TAKE CONTROL.
    TOP = open ;  //start the threads
    WAIT_FOR_THREADS() ;  //wait for the threads/handshake to finish

//STAGE THREE//
   //AT THIS POINT, THE MAIN HAS CONTROL AGAIN.
   //THE THREADS ARE WAITING FOR THE BOTTOM TO OPEN.
    TOP = closed ;  //close the top of the threads
    RESET_FLAG() ;
    BOTTOM = open ;  //allow the threads to reset, they'll wait at the (closed) top
    STAGE_THREE() ;  //post-handshake Fastjet computations
    firstevent = false ; //well, it's not the first event anymore
  }
}









//-------------------------------------------------------
void handshake_algorithm()  //THIS GETS PASSED TO THE THREADS
{
  while( true ) // LOOP THIS PROCESS FOR MANY EVENTS
  {
    TELL_MAIN_IM_READY() ;  //tell the MAIN you're waiting at the top
    while( TOP == closed ) {/*  WAIT HERE  */}

    STAGE_TWO() ;  //execute the handshake

    while( BOTTOM == closed ) {/*  WAIT HERE  */}
  }
}
//-------------------------------------------------------
void TELL_MAIN_IM_READY()
{
  _mtx.lock() ;
  flag-- ;  //decrement flag, since main's waiting for flag=0 (meaning, zero threads are still working)
  _mtx.unlock() ;
}
//-------------------------------------------------------
void WAIT_FOR_THREADS()
{
  while( flag > 0 ) {/*  WAIT HERE  */} //if threads are still working, wait here
}
//-------------------------------------------------------
//   Obviously, instead of cout-ing, we'd replace these with sections of FastJet code
void STAGE_ONE() //PRE-HANDSHAKE STUFF
{
  cout << "START OF EVENT " << eventnumber << ":" << endl ;
}

void STAGE_TWO() //HANDSHAKE STUFF
{
  _mtx.lock() ;
  flag-- ;
  cout << "     NUMBER OF THREADS WORKING: " << flag << endl ;
  _mtx.unlock() ;
}

void STAGE_THREE() //POST-HANDSHAKE STUFF
{
  cout << "EVENT " << eventnumber << " IS DONE" << endl << endl ;
}
//-------------------------------------------------------
void RESET_FLAG()
{
  _mtx.lock() ;
  flag = NUMBER_OF_THREADS ;
  _mtx.unlock() ;
}
//-------------------------------------------------------
void CREATE_THREADS( void (*handshake_algorithm)() )
{
  for ( int i = 0 ; i < NUMBER_OF_THREADS ; i++ )
  {
    TGROUP.create_thread( (*handshake_algorithm) ) ; 
  }
}
//--END-OF-FILE--//--------------------------------------






