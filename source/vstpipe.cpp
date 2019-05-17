#include "vstpipe.h"
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
}

//-------------------------------------------------------------------------------------------------------
VstPipe::~VstPipe ()
{
  DisconnectNamedPipe(pipe);
  DisconnectNamedPipe(dbg_pipe);
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

    // if closed, disconnect pipe
    LPDWORD numBytesWritten = 0;
    DWORD nRead, nTotal, nLeft;
    PeekNamedPipe(pipe, buf, 8192, &nRead, &nTotal, &nLeft);
    if (nRead > 0) {
      DisconnectNamedPipe(pipe);
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
    }

    DEBUG("works");
 
    // write audio
    numBytesWritten = 0;
    WriteFile(
      pipe,
      buf,
      2 * bufferSize * sizeof(float),
      numBytesWritten,
      NULL // not using overlapped IO
      );

    // write debug msg
    if (dbg_buf[0] != NULL) {
      numBytesWritten = 0;
      WriteFile(
        dbg_pipe,
        dbg_buf,
        strlen(dbg_buf),
        numBytesWritten,
        NULL // not using overlapped IO
        );
      memset(dbg_buf, 0, sizeof(dbg_buf));
    }
}

void VstPipe::DEBUG(char msg[]) {
  vst_strncat(dbg_buf, msg, 8192);
}