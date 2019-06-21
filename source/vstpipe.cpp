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
  memset(local_buf, 0, sizeof(local_buf));
  memset(remote_buf, 0, sizeof(remote_buf));
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

    float *local_buf_ptr = local_buf;
    float *remote_buf_ptr = remote_buf;

    VstInt32 buffer_size = sampleFrames;
    VstInt32 leftSamples = sampleFrames;
    // interleave VST input
    while (--leftSamples >= 0) {
      (*local_buf_ptr++) = (*in1);
      (*local_buf_ptr++) = (*in2);
    }

    // audio is sent/received as interleaved stereo
    audio_pipe->sendData<uint8_t>(PipeCommand::AUDIO_PROCESS_COMMAND);
    audio_pipe->process(local_buf, remote_buf, buffer_size);

    while (--sampleFrames >= 0) {
      // threshold noise
      float r = (float) rand() / RAND_MAX;
			r = 1.f - 2.f * r;
			r *= 1.5e-5f;

      if (remote_buf[0] == 0) {
        (*out1++) = (*in1++) + r;
        (*out2++) = (*in2++) + r;
      } else {
        (*out1++) = (*remote_buf_ptr++);
        (*out2++) = (*remote_buf_ptr++);
      }
    }
}

void VstPipe::DEBUG(char msg[]) {
  if (strlen(dbg_buf) < 1023) {
    vst_strncat(dbg_buf, msg, 1023);
  }
}