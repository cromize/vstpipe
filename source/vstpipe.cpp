#include <stdio.h>
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

  /*
  // init audio pipe
  memset(buf, 0, sizeof(buf));
  pipe = CreateNamedPipe(
    "\\\\.\\pipe\\vstpipe1",
    PIPE_ACCESS_DUPLEX,
    PIPE_TYPE_BYTE | PIPE_NOWAIT,
    255,
    0,
    0,
    1, 
    NULL 
  );
*/
  /*
  // init debug pipe
  memset(dbg_buf, 0, sizeof(dbg_buf));
  dbg_pipe = CreateNamedPipe(
    "\\\\.\\pipe\\vstpipedebug",
    PIPE_ACCESS_DUPLEX,
    PIPE_TYPE_BYTE | PIPE_NOWAIT,
    255,
    0,
    0,
    1, 
    NULL 
  );
  */
  dbg_pipe = new Pipe(0);
  dbg_pipe->init();
  memset(dbg_buf, 0, sizeof(dbg_buf));
  DEBUG("[VST] * debug pipe init\n");

  audio_pipe = new Pipe(1);
  audio_pipe->init();
  memset(buf, 0, sizeof(buf));
  DEBUG("[VST] * audio pipe init\n");
}

//-------------------------------------------------------------------------------------------------------
VstPipe::~VstPipe ()
{
  delete audio_pipe;
  delete dbg_pipe;
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
	vst_strncpy (text, "cromize (c) 2019", kVstMaxVendorStrLen);
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

    VstInt32 bufferSize = sampleFrames;
    while (--sampleFrames >= 0)
    {
        (*buf_ptr++) = (*in1);
        (*buf_ptr++) = (*in2);

        (*out1++) = (*in1++);
        (*out2++) = (*in2++);
    }

    // kill broken pipe
    audio_pipe->check_broken_pipe();

    // send audio data in non-blocking mode
    audio_pipe->send_data(buf, 2 * bufferSize * sizeof(float));

    // write debug msg
    if (dbg_buf[0] != NULL) {
      dbg_pipe->send_data(dbg_buf, strlen(dbg_buf));

      // if client received msg, clean buffer
      DWORD nRead, nTotal, nLeft;
      PeekNamedPipe(dbg_pipe->get_pipe(), 0, 1024, &nRead, &nTotal, &nLeft);
      if (nTotal == 0) {
        memset(dbg_buf, 0, sizeof(dbg_buf));
      }
    }
}

void VstPipe::DEBUG(char msg[]) {
  if (strlen(dbg_buf) < 1023) {
    vst_strncat(dbg_buf, msg, 1023);
  }
}