

//#include "fastjet/concurrency/TiledN2.cc"
#include "TiledN2.cc"


vector<Tile> _ptiles ;
vector<boost::mutex*> _tile_mutex ;



bool TOP = false ;       // TOP regulates the flow of the thread
bool BOTTOM = false ;    // BOTTOM also regulates thread flow


void SEND_TILES_TO_THREADS( vector<Tile> _tiles ) ;
// used within _initialise_tiles() to create threads

void CLEAN_TILES( vector<Tile> _tiles ) ;
// used within ClusterSequence_TiledN2.cc to
// clear previous event's jets/tags from tiles

void WAIT_FOR_THREADS() ;
// used to ensure thread synchronization
//  (1) after _initialise_tiles()
//  (2) at NN_INIT_LOOP

void RESET_FLAG() ;

void TELL_MAIN_IM_READY() ;

void NN_INITIALIZATION( Tile * tilee ) ;

