#include <stdio.h>
#include <thread>
#include "vstpipe.h"
#include "pipe.h"
#include "windows.h"

//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new VstPipe (audioMaster);
}

//-------------------------------------------------------------------------------------------------------
VstPipe::VstPipe (audioMasterCallback audioMaster)
: AudioEffectX (audioMaster, 1, 1)	// 1 program, 1 parameter only
{
	setNumInputs (2);		// stereo in
	setNumOutputs (2);		// stereo out
	setUniqueID ('Pipe');	// identify
	canProcessReplacing ();	// supports replacing output
	vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name

  // init audio pipe
  audio_pipe = new Pipe();
  audio_pipe->init();
  memset(buf, 0, sizeof(buf));
}

//-------------------------------------------------------------------------------------------------------
VstPipe::~VstPipe ()
{
  delete audio_pipe;
}

//-------------------------------------------------------------------------------------------------------
void VstPipe::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void VstPipe::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void VstPipe::setParameter (VstInt32 index, float value)
{
}

//-----------------------------------------------------------------------------------------
float VstPipe::getParameter (VstInt32 index)
{
  return 0.0f;
}

//-----------------------------------------------------------------------------------------
void VstPipe::getParameterName (VstInt32 index, char* label)
{
}

//-----------------------------------------------------------------------------------------
void VstPipe::getParameterDisplay (VstInt32 index, char* text)
{
}

//-----------------------------------------------------------------------------------------
void VstPipe::getParameterLabel (VstInt32 index, char* label)
{
}

//------------------------------------------------------------------------
bool VstPipe::getEffectName (char* name)
{
	vst_strncpy (name, "VstPipe", kVstMaxEffectNameLen);
	return true;
}

//------------------------------------------------------------------------
bool VstPipe::getProductString (char* text)
{
	vst_strncpy (text, "VstPipe", kVstMaxProductStrLen);
	return true;
}

//------------------------------------------------------------------------
bool VstPipe::getVendorString (char* text)
{
	vst_strncpy (text, "VstPipe", kVstMaxVendorStrLen);
	return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 VstPipe::getVendorVersion ()
{ 
	return 1000; 
}

//-----------------------------------------------------------------------------------------
void VstPipe::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* in1  =  inputs[0];
    float* in2  =  inputs[1];
    float* out1 = outputs[0];
    float* out2 = outputs[1];

    float *buf_ptr = buf;

    // audio is sent as interleaved stereo
    VstInt32 buffer_size = sampleFrames;
    while (--sampleFrames >= 0) {
      (*buf_ptr++) = (*in1);
      (*buf_ptr++) = (*in2);

      (*out1++) = (*in1++);
      (*out2++) = (*in2++);
    }

    audio_pipe->sendData<uint8_t>(PipeCommand::AUDIO_PROCESS_COMMAND);
    audio_pipe->process(buf, 0, buffer_size);
}

void VstPipe::DEBUG(char msg[]) {
  if (strlen(dbg_buf) < 1023) {
    vst_strncat(dbg_buf, msg, 1023);
  }
}