//
//  CoreAudioInputProcessor.h
//

#import <Cocoa/Cocoa.h>
#import <MacRuby/MacRuby.h>

#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>

#include <queue>

template <typename T>
class RingBuffer{
public:
	RingBuffer(unsigned long num = 0){
		m_pBuffer  = NULL;
		reset(num);
	}
	
	void reset(unsigned long num){
		if (m_pBuffer) delete [] m_pBuffer;
		m_pBuffer = NULL;
		m_pBuffer = new T[num];
		m_size = num;
		m_writePos = 0;
		m_readPos = 0;
		m_writeRotationCount = 0;
		m_readRotationCount = 0;
	}
	
	unsigned long size(){
		return m_size;
	}
	
	unsigned long byteSize(){
		return m_size + sizeof(T);
	}
	
	bool write(const T *pBuffer, unsigned long num){
		if ( m_writePos + num < m_size){
			T *pCurrent = m_pBuffer + m_writePos;
			memcpy(pCurrent, pBuffer, num * sizeof(T));
			m_writePos += num;
			//NSLog(@"wrote");
			return true;
		}
		
		if ( m_writePos + num >= m_size){
			//とりあえず、全部書いて、あとは再帰させる
			T *pCurrent = m_pBuffer + m_writePos;
			memcpy(pCurrent, pBuffer, sizeof(T) *( m_size - m_writePos) );
			unsigned long writtenNum = m_size - m_writePos;
			m_writePos = 0;
			m_writeRotationCount++;
			//NSLog(@"ring full.");
			return write(pBuffer + writtenNum, num - writtenNum);
		}
	}
	
	bool read(T *pBuffer, unsigned long num){
		/*NSLog(@"read called rotation:%lu,%lu. pos=%lu, %lu",
			  m_writeRotationCount, m_readRotationCount, m_writePos,m_readPos);
		*/
		static int count = 0;
		if (count % 5000 == 0){
			NSLog(@"buffer status behind=%.8lu readPos=%lu,%lu, writePos=%lu,%lu",
				  m_writePos - m_readPos,
				  m_readRotationCount, m_readPos, 
				  m_writeRotationCount, m_writePos);
		}
		count++;
		if (m_writeRotationCount - m_readRotationCount >= 2){
			if (m_writePos >= 2048){
				m_readRotationCount = m_writeRotationCount;
				m_readPos = m_writePos - 2048;
			}else{
				m_readRotationCount = m_writeRotationCount - 1;
				m_readPos = m_size - (2048 - m_writePos);
			}
			NSLog(@"behind solved");
		}else if (m_writeRotationCount - m_readRotationCount == 1){
			
			if (m_writePos + (m_size - m_readPos) > 4048){
				if (m_writePos >= 2048){
					m_readRotationCount = m_writeRotationCount;
					m_readPos = m_writePos - 2048;
				}else{
					m_readPos = m_size - (2048 - m_writePos);
				}
			}
		}else if ( m_writePos - (m_readPos + num) > 4048){
			NSLog(@"buffer overrun detected");
			m_readPos = m_writePos - 2048;
		}
		
		
		if (m_readRotationCount > m_writeRotationCount){
			NSLog(@"buffer underran 2");
			return false;
		}else if (m_readRotationCount == m_writeRotationCount){
			if (m_writePos < m_readPos + num){
				NSLog(@"buffer underran 2");
				return false;
			}
		}

				
		if ( m_readPos + num < m_size){
			T *pCurrent = m_pBuffer + m_readPos;
			memcpy( pBuffer ,pCurrent, num * sizeof(T));
			m_readPos += num;
			
			//NSLog(@"read done write=%lu,read=%lu\n", m_writePos, m_readPos);
			return true;
		}
		
		if (m_readPos + num >= m_size){
			T *pCurrent = m_pBuffer + m_readPos;
			memcpy(pBuffer, pCurrent, sizeof(T) * (m_size - m_readPos));
			unsigned long readedNum = m_size - m_readPos;
			m_readPos = 0;
			m_readRotationCount++;
			return read(pBuffer + readedNum , num - readedNum);
		}
	}
	
private:
	
	//nums.not pointers
	unsigned long m_size;
	unsigned long m_writePos;
	unsigned long m_writeRotationCount;
	unsigned long m_readPos;
	unsigned long m_readRotationCount;
	
	//the pointer to top
	T *m_pBuffer;
	
};
	

@interface CoreAudioInputProcessor : NSObject {
	AudioUnit _inputUnit;
	AudioBufferList *_tempBufferList;
	RingBuffer<float> left;	//TODO: Make this ring-buffer
	RingBuffer<float> right;
}

-(bool) initProcessor;
-(bool) start;
-(bool) stop;

-(RingBuffer<float> *) left;
-(RingBuffer<float> *) right;

- (OSStatus) inputComming:(AudioUnitRenderActionFlags *)ioActionFlags :(const AudioTimeStamp *) inTimeStamp:
(UInt32) inBusNumber: (UInt32) inNumberFrames :(AudioBufferList *)ioData;


@end
