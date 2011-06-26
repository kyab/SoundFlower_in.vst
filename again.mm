//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
// Version 2.4		$Date: 2006/11/13 09:08:27 $
//
// Category     : VST 2.x SDK Samples
// Filename     : again.cpp
// Created by   : Steinberg Media Technologies
// Description  : Stereo plugin which applies Gain [-oo, 0dB]
//
// � 2006, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#import "again.h"
#import <Cocoa/Cocoa.h>

//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new AGain (audioMaster);
}

//-------------------------------------------------------------------------------------------------------
AGain::AGain (audioMasterCallback audioMaster)
: AudioEffectX (audioMaster, 1, 2), sin(440,0.5)	// 1 program, 1 parameter only
{
	setNumInputs (2);		// stereo in
	setNumOutputs (2);		// stereo out
	setUniqueID ('kysf');	// identify
	canProcessReplacing ();	// supports replacing output
	//canDoubleReplacing ();	// supports double precision processing

	fGain = 1.f;			// default to 0 dB
	vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
	processor = nil;
	printf("constructor called\n");	//called by scan.
}

//-------------------------------------------------------------------------------------------------------
AGain::~AGain ()
{
	[processor stop];
	// nothing to do here
	printf("destructor called\n");	//called by scan.
}

void AGain::open(){
	printf("open()\n");
	NSLog(@"foo bar baz");
	
	char hostVender[64];
	char hostProduct[64];
	getHostVendorString(hostVender);
	getHostProductString(hostProduct);
	VstInt32 rawVersion = getHostVendorVersion();
	NSLog(@"VstHost:%s %s(%d)",hostVender, hostProduct, rawVersion);
	
	//here is the chance to initialize out input engine
	
	processor = [[CoreAudioInputProcessor alloc] init];
	bool result = [processor initProcessor];
	if (!result){
		NSLog(@"failed to init processor");
	}else{
		NSLog(@"succeeded to init processor");
	}
	
	result = [processor start];
	if (!result){
		NSLog(@"failed to start processor");
	}
}

void AGain::close(){
	[processor stop];
	NSLog(@"close()");
}

void AGain::suspend(){
	NSLog(@"suspend()");
}

void AGain::resume(){
	NSLog(@"resume()");
}


//-------------------------------------------------------------------------------------------------------
void AGain::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}



//-----------------------------------------------------------------------------------------
void AGain::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void AGain::setParameter (VstInt32 index, float value)
{
	switch(index){
		case 0:
			fGain = value;
			break;
		case 1:
			sin.setFreq(440 + (int)(440*value));
			break;
	}
}

//-----------------------------------------------------------------------------------------
float AGain::getParameter (VstInt32 index)
{
	switch(index){
		case 0:
			return fGain;
			break;
		case 1:
			return (sin.freq() - 440.0f)/440.0f;
			break;
	}
	return 0;
}

//-----------------------------------------------------------------------------------------
void AGain::getParameterName (VstInt32 index, char* label)
{
	switch(index){
		case 0:
			vst_strncpy (label, "GainKy", kVstMaxParamStrLen);
			break;
		case 1:
			vst_strncpy (label, "Freq", kVstMaxParamStrLen);
			break;
		default:
			break;
	}
	
}

//-----------------------------------------------------------------------------------------
void AGain::getParameterDisplay (VstInt32 index, char* text)
{
	switch(index){
		case 0:
			dB2string (fGain, text, kVstMaxParamStrLen);
			break;
		case 1:
			int2string(sin.freq(), text, kVstMaxParamStrLen);
			break;
	}
}

//-----------------------------------------------------------------------------------------
void AGain::getParameterLabel (VstInt32 index, char* label)
{
	switch(index){
		case 0:
			vst_strncpy (label, "dB", kVstMaxParamStrLen);
			break;
		case 1:
			vst_strncpy (label, "Hz", kVstMaxParamStrLen);
			break;
	}
}

//------------------------------------------------------------------------
bool AGain::getEffectName (char* name)
{
	vst_strncpy (name, "SFSF", kVstMaxEffectNameLen);
	return true;
}

//------------------------------------------------------------------------
bool AGain::getProductString (char* text)
{
	vst_strncpy (text, "SFSF(Product)", kVstMaxProductStrLen);
	return true;
}

//------------------------------------------------------------------------
bool AGain::getVendorString (char* text)
{
	vst_strncpy (text, "kyab", kVstMaxVendorStrLen);
	return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 AGain::getVendorVersion ()
{ 
	return 10221; 
}

//-----------------------------------------------------------------------------------------
void AGain::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* in1  =  inputs[0];
    float* in2  =  inputs[1];
    float* out1 = outputs[0];
    float* out2 = outputs[1];
	
	RingBuffer<float> *left = [processor left];
	RingBuffer<float> *right = [processor right];
	
	@synchronized(processor){
		//*out1++ = *in1++;
		left->read(out1, sampleFrames);
		right->read(out2, sampleFrames);
	}
}

//-----------------------------------------------------------------------------------------
void AGain::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{
    double* in1  =  inputs[0];
    double* in2  =  inputs[1];
    double* out1 = outputs[0];
    double* out2 = outputs[1];
	double dGain = fGain;

    while (--sampleFrames >= 0)
    {
        (*out1++) = (*in1++) * dGain;
        (*out2++) = (*in2++) * dGain;
    }
}
