//
//  util.h
//  AiffPlayer
//
//  Created by koji on 11/01/06.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef __UTIL_H__
#define __UTIL_H__

#import <Cocoa/Cocoa.h>

#include <string>
#include <typeinfo>


//demangle function
//http://d.hatena.ne.jp/hidemon/20080731/1217488497
#include <string>



//convert enum four cc to human readable
NSString *EnumToFOURCC(UInt32 val);



// macro IF_FAILED(result,message){...}

/*
 usage:
 OSStatus result = SomeAPIReturnsOSStatus(....);
 IF_FAILED(result, "uups failed to SomeAPIReturnsOSStatus")){
 return ;
 }
 
 //in above code . block only exeuted if result != noErr.
 */

#define FAILED_BLOCK(result,message) if ( FailChecker checker = FailChecker(result,message) ){} else
#define IF_FAILED(result,message) if ( FailChecker checker = FailChecker(result,message) ){} else

class FailChecker{
public:
	FailChecker(OSStatus result, const char *msg){
		_result = result;
		if (_result != noErr){
			NSLog(@"%s. err=%d", msg, _result);
		}
	}
	operator bool (){
		return (_result == noErr);
	}
	
private:
	OSStatus _result;
};

//benchmark
class Timer{
public:
	Timer(){
	}
	
	void start(){
		_startTime = CFAbsoluteTimeGetCurrent();
	}
	
	void stop(){
		_endTime = CFAbsoluteTimeGetCurrent();
	}
	
	CFAbsoluteTime result(){
		return _endTime - _startTime;
	}
private:
	CFAbsoluteTime _startTime, _endTime;
};


#endif //__UTIL_H__
