/*
 * Author: Harry van Haaren 2014
 *         harryhaaren@gmail.com
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "pad.hxx"

#include "sample.hxx"
#include <stdio.h>
#include <string.h>

#ifdef FABLA2_COMPONENT_TEST
#include "tests/qunit.hxx"
extern QUnit::UnitTest qunit;
#endif 


namespace Fabla2
{

Pad::Pad( Fabla2DSP* d, int rate, int ID ) :
  dsp( d ),
  sr(rate),
  ID_( ID ),
  muteGroup_( 0 ),
  
  sampleSwitchSystem( SS_NONE ),
  sampleLayerCounter(0)
{
#ifdef FABLA2_COMPONENT_TEST
  printf("%s\n", __PRETTY_FUNCTION__ );
#endif
  
  samples.reserve( 4 );
}

void Pad::add( Sample* s )
{
  samples.push_back( s );
  printf( "Pad::add() %s, total #samples on pad = %i\n", s->getName(), samples.size() );
}

Sample* Pad::layer( int id )
{
  if( id < samples.size() && id >= 0 )
    return samples.at(id);
  return 0;
}

int Pad::lastPlayedLayer()
{
  return sampleLayerCounter;
}

void Pad::checkAll()
{
  printf("%s : Starting...\n", __PRETTY_FUNCTION__ );
  for(int bi = 0; bi < 8; bi++ )
  {
    Sample* s = layer( bi );
    if( !s )
    {
      printf("%s : Pad::layer( %i ) returns NULL sample\n", __PRETTY_FUNCTION__, bi );
      return;
    }
    else
    {
      printf("%s : Pad::layer( %i ) has Sample %s\n", __PRETTY_FUNCTION__, bi, s->getName() );
    }
  }
  printf("%s : Done.\n", __PRETTY_FUNCTION__ );
}

Sample* Pad::getPlaySample( int velocity )
{
#ifdef FABLA2_COMPONENT_TEST
  QUNIT_IS_TRUE( samples.size() > 0 );
#endif
  
  /// Logic to do round-robin / velocity mapping here
  if( samples.size() > 0 )
  {
    // first update the sample-counter, wrap it if needed. Later we play the
    // sample we just updated to, and its stored in sampleLayerCounter for UI
    sampleLayerCounter++;
    if( sampleLayerCounter >= samples.size() )
        sampleLayerCounter = 0;
    
    if( sampleSwitchSystem == SS_NONE )
    {
      sampleLayerCounter = 0;
      return samples.at( 0 );
    }
    else if( sampleSwitchSystem == SS_ROUND_ROBIN )
    {
      Sample* tmp = samples.at( sampleLayerCounter );
      
      if( sampleLayerCounter >= samples.size() )
        sampleLayerCounter = 0;
      
      return tmp;
    }
    else if( sampleSwitchSystem == SS_VELOCITY_LAYERS )
    {
      // iter trough samples, return first that was add()-ed that applies
      for(int i = 0; i < samples.size(); i++ )
      {
        if( samples.at(i)->velocity( velocity ) )
        {
          // remember last played layer, for UI updates
          sampleLayerCounter = i;
          return samples.at(i);
        }
      }
      Sample* tmp = samples.at( sampleLayerCounter );
      return tmp;
    }
  }
  
  // if no sample is loaded, or a velocity outside the mapped regions is played
  return 0;
}

void Pad::clearAllSamples()
{
  samples.clear();
}

void Pad::switchSystem( SAMPLE_SWITCH_SYSTEM ss )
{
  sampleSwitchSystem = ss;
}

Pad::~Pad()
{
#ifdef FABLA2_COMPONENT_TEST
  printf("%s\n", __PRETTY_FUNCTION__ );
#endif
}

};
