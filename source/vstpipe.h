#ifndef __vstpipe__
#define __vstpipe__

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "windows.h"
#include "pipe.h"

//-------------------------------------------------------------------------------------------------------
class VstPipe : public AudioEffectX
{
public:
	VstPipe (audioMasterCallback audioMaster);
	~VstPipe ();

	// Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);

	// Program
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);

	// Parameters
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();

  virtual void VstPipe::DEBUG(char msg[]);

protected:
  Pipe *audio_pipe;
  char dbg_buf[1024];
  float local_buf[16384];
  float remote_buf[16384];
	char programName[kVstMaxProgNameLen + 1];
};

#endif
