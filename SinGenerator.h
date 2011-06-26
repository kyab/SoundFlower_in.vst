/*
 *  SinGenerator.h
 *  SoundFlower_in
 *
 *  Created by koji on 11/06/22.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <math.h>	//sin,M_PI
#include <limits.h>
#include <CoreFoundation/CoreFoundation.h>
//#import <Foundation/Foundation.h>



//サイン波のgenerator
class SinGenerator{
	
public:
	SinGenerator(UInt32 freq, float gain){
		m_currentSampleCount = 0;
		m_freq = freq;	//Hz
		m_gain = gain;
		m_prevPhase = 0;
		//NSLog(@"SinGenerator::ctor(0x%.8x) constructor, freq=%u",this,m_freq);
	}
	
	void reset(){
		m_currentSampleCount = 0;
	}
	
	void setFreq(UInt32 freq){
		m_freq = freq;
	}
	
	void setGain(float gain){
		if (gain > 1.0) gain = 1.0;
		if (gain < 0.0) gain = 0.0;
		m_gain = gain;
	}
	
	UInt32 freq(){
		return m_freq;
	}
	
	//generate one sample (assume freq never change)
	
	SInt16 gen(){
		double current_sec = (double)m_currentSampleCount/44100;
		
		//角速度 = rad/s
		double rspeed = m_freq*2*M_PI;
		
		//val = 角速度×時間
		float val = (float)m_gain*SHRT_MAX *sin(rspeed*current_sec);
		
		m_currentSampleCount++;
		//printf("%lf\n",val);
		return (SInt16)val ;
	}
	
	//support dynamic freq change
	SInt16 gen2(){
		
		double rspeed = m_freq*2*M_PI;
		
		double rad = m_prevPhase + rspeed*1/44100; 
		float val = (float)m_gain*SHRT_MAX * sin(rad);
		m_prevPhase = rad;
		
		m_currentSampleCount++;
		return (SInt16)val;										
	}
	
private:
	UInt32 m_currentSampleCount;
	UInt32 m_freq;
	float m_gain;
	
	double m_prevPhase;	//[rad]
};
