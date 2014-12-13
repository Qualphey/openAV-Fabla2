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

#include "voice.hxx"

#include "plotter.hxx"
#include "sampler.hxx"
#include "fabla2.hxx"
#include <math.h>

namespace Fabla2
{

Voice::Voice( Fabla2DSP* d, int r ) :
  dsp( d ),
  sr ( r ),
  active_( false )
{
  adsr = new ADSR();
  
  adsr->setAttackRate  ( 1.0 * r );
  adsr->setDecayRate   ( 1.0 * r );
  adsr->setSustainLevel( 0.5  );
  adsr->setReleaseRate ( 0.5 * r );
}

void Voice::play()
{
  active_ = true;
  phase = 0;
  
  adsr->reset();
  adsr->gate( true );
  
  if( true )
  {
    std::vector<float> tmp(44100 * 5);
    int i = 0;
    for( i = 0; i < 44100 * 3; i++ )
    {
      tmp.at(i) = adsr->process();
    }
    
    adsr->gate( false );
    
    for( ; i < 44100 * 5; i++ )
    {
      tmp.at(i) = adsr->process();
    }
    
    Plotter::plot( "adsr.dat", 44100 * 5, &tmp[0] );
    
  }
  
}

/*
void Voice::stop()
{
  adsr->gate( false );
}
*/

void Voice::process()
{
  float* outL = dsp->controlPorts[OUTPUT_L];
  float* outR = dsp->controlPorts[OUTPUT_R];
  
  for( int i = 0; i < dsp->nframes; i++ )
  {
    float adsrValue = adsr->process();
    float freq = 40 + 400.f * *dsp->controlPorts[MASTER_VOL];
    
    const float sampsPerCycle = sr / 110;
    const float phaseInc = (1.f / sampsPerCycle);
    
    const float tmp = sin( phase * 2 * 3.1415 ) * 0.2 * adsrValue;
    
    *outL++ += tmp;
    *outR++ += tmp;
    
    phase += phaseInc;
  }
}

Voice::~Voice()
{
}


}; // Fabla2
