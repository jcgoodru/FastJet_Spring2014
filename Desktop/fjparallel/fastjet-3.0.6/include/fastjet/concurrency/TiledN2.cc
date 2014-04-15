



void SEND_TILES_TO_THREADS( vector<Tile> _tiles )
{
  flag = _tiles.size() ;
  for ( int i = 0 ; i < _tiles.size() ; i++ )
  {
    Tile * tilee = &_tiles[i] ;
    (*tilee).address = i ;
    boost::mutex _mtxqaz ;
    _tile_mutex.push_back( _mtxqaz ) ;

    TGROUP.create_thread( boost::bind( &NN_INITIALIZATION , tilee ) ) ;
  }
}

void CLEAN_TILES() // remove bj from tiles???
{
  for int ( i = 0 ; i < _ptiles.size ; i++ )
  {
    Tile * tilee = &_ptiles[i] ;
    *tilee = NULL ;
    (*tilee).tagged = false ;
  }
}

void WAIT_FOR_THREADS()
{
  while( flag > 0 ) {/*  WAIT HERE  */} //if threads are still working, wait here
}

void RESET_FLAG()
{
  _mtx.lock() ;
  flag = _tiles.size() ;
  _mtx.unlock() ;
}

void TELL_MAIN_IM_READY()
{
  _mtx.lock() ;
  flag-- ;  //decrement flag, since main's waiting for flag=0 (meaning, zero threads are still working)
  _mtx.unlock() ;
}

void NN_INITIALIZATION( Tile * tilee )
{
  /cout << "W" ; //sees if a thread is actually working
  TiledJet * jetA ;
  TiledJet * jetB ;
  while( true ) // LOOP THIS PROCESS FOR MANY EVENTS
  {
    TELL_MAIN_IM_READY() ;  //tell the MAIN you're waiting at the top
    while( TOP == false ) {/*  WAIT HERE  */}

      _tile_mutex[ tilee->address ]->lock() ;
      // first do it on this tile
      for (jetA = tilee->head; jetA != NULL; jetA = jetA->next) {
        for (jetB = tilee->head; jetB != jetA; jetB = jetB->next) {
	  double dist = _bj_dist(jetA,jetB);
          if (dist < jetA->NN_dist) {jetA->NN_dist = dist; jetA->NN = jetB;}
	  if (dist < jetB->NN_dist) {jetB->NN_dist = dist; jetB->NN = jetA;}
        }
      }
      _tile_mutex[ tilee->address ]->unlock() ;

      // then do it for RH tiles
      for (Tile ** RTilee = tilee->RH_tiles; RTilee != tilee->end_tiles; RTilee++) {
        int locks = 0 ;
        while ( locks < 2 )
        {
          locks = 0 ;
          if ( _tile_mutex[ tilee->address ]->try_lock() )
            { locks++ ; }
          if ( _tile_mutex[ (*Rtilee)->address ]->try_lock() )
            { locks++ ; }
          if ( locks != 2 )
            {
              _tile_mutex[ tilee->address ]->unlock() ;
              _tile_mutex[ (*Rtilee)->address ]->unlock() ;
            }
        }
        for (jetA = tilee->head; jetA != NULL; jetA = jetA->next) {
	  for (jetB = (*RTilee)->head; jetB != NULL; jetB = jetB->next) {
	    double dist = _bj_dist(jetA,jetB);
  	    if (dist < jetA->NN_dist) {jetA->NN_dist = dist; jetA->NN = jetB;}
	    if (dist < jetB->NN_dist) {jetB->NN_dist = dist; jetB->NN = jetA;}
	  }
        }
        _tile_mutex[ tilee->address ]->unlock() ;
        _tile_mutex[ (*Rtilee)->address ]->unlock() ;
      }
    }
    while( BOTTOM == false ) {/*  WAIT HERE  */}
  }
  /cout << "d" ; //sees if a thread is actually working
}


