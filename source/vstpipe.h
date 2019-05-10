#ifndef __vstpipe__
#define __vstpipe__

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "windows.h"

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

protected:
  HANDLE pipe;
  float buf[8192];
	char programName[kVstMaxProgNameLen + 1];
};

#endif
