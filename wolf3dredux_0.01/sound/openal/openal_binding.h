/*
 *	Copyright (C) 2004 Michael Liebscher
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version 2
 *	of the License, or (at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*
 *	openal_binding.h: Interface to OpenAL library.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 */


#ifndef __OPENAL_BINDING_H__
#define __OPENAL_BINDING_H__


#include "AL/alc.h"

#include "../../common/arch.h"

/* prototypes for functions from openal_binding.c: */
extern _boolean OpenAL_Init(const char *dllname);
extern void OpenAL_Shutdown(void);

/* prototypes for functions from openal_main.c: */
extern _boolean Sound_Device_Setup(void);
extern void Sound_Device_Shutdown(void);

/* typedefs (?): */
typedef ALCAPI ALCvoid (ALCAPIENTRY * ALCCLOSEDEVICE)(ALCdevice *pDevice);
typedef ALCAPI ALCcontext*(ALCAPIENTRY * ALCCREATECONTEXT)(ALCdevice *device,
														   const ALCint *attrList);
typedef ALCAPI ALCvoid (ALCAPIENTRY * ALCDESTROYCONTEXT)(ALCcontext *context);
typedef ALCAPI ALCdevice* (ALCAPIENTRY * ALCGETCONTEXTSDEVICE)(ALCcontext *pContext);
typedef ALCAPI ALCcontext * (ALCAPIENTRY * ALCGETCURRENTCONTEXT)(ALCvoid);
typedef ALCAPI ALCenum (ALCAPIENTRY * ALCGETENUMVALUE)(ALCdevice *device,
													   const ALCubyte *enumName);
typedef ALCAPI ALCenum (ALCAPIENTRY * ALCGETERROR)(ALCdevice *device);
typedef ALCAPI ALCvoid (ALCAPIENTRY * ALCGETINTEGERV)(ALCdevice *device,
													  ALCenum param,
													  ALsizei size,
													  ALCint *data);
typedef ALCAPI ALCvoid *  (ALCAPIENTRY * ALCGETPROCADDRESS)(ALCdevice *device,
															const ALCubyte *funcName);
typedef ALCAPI const ALCubyte* (ALCAPIENTRY * ALCGETSTRING)(ALCdevice *device,
															ALCenum param);
typedef ALCAPI ALCboolean (ALCAPIENTRY * ALCISEXTENSIONPRESENT)(ALCdevice *device,
																const ALCubyte *extName);
typedef ALCAPI ALCboolean (ALCAPIENTRY * ALCMAKECONTEXTCURRENT)(ALCcontext *context);
typedef ALCAPI ALCdevice* (ALCAPIENTRY * ALCOPENDEVICE)(const ALCubyte *deviceName);
typedef ALCAPI ALCvoid (ALCAPIENTRY * ALCPROCESSCONTEXT)(ALCcontext *pContext);
typedef ALCAPI ALCvoid (ALCAPIENTRY * ALCSUSPENDCONTEXT)(ALCcontext *pContext);



typedef ALCAPI ALvoid (ALCAPIENTRY * ALGENBUFFERS)(ALsizei n, ALuint* bufferNames);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALDELETEBUFFERS)(ALsizei n, const ALuint* bufferNames);
typedef ALCAPI ALboolean (ALCAPIENTRY * ALISBUFFER)(ALuint bufferName);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALBUFFERDATA)(ALuint bufferName, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETBUFFERF)(ALuint bufferName, ALenum param, ALfloat* value);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETBUFFERI)(ALuint bufferName, ALenum param, ALint* value);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALENABLE)(ALenum capability);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALDISABLE)(ALenum capability);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALDOPPLERFACTOR)(ALfloat value);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALDOPPLERVELOCITY)(ALfloat value);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALDISTANCEMODEL)(ALenum value);
typedef ALCAPI ALboolean (ALCAPIENTRY * ALGETBOOLEAN)(ALenum param);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETBOOLEANV)(ALenum param, ALboolean* data);
typedef ALCAPI ALdouble (ALCAPIENTRY * ALGETDOUBLE)(ALenum param);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETDOUBLEV)(ALenum param, ALdouble* data);
typedef ALCAPI ALfloat (ALCAPIENTRY * ALGETFLOAT)(ALenum param);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETFLOATV)(ALenum param, ALfloat* data);
typedef ALCAPI ALint (ALCAPIENTRY * ALGETINTEGER)(ALenum param);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETINTEGERV)(ALenum param, ALint* data);
typedef ALCAPI ALenum (ALCAPIENTRY * ALGETENUMVALUE)(const ALubyte* ename);
typedef ALCAPI ALenum (ALCAPIENTRY * ALGETERROR)(ALvoid);
typedef ALCAPI ALvoid* (ALCAPIENTRY * ALGETPROCADDRESS)(const ALubyte* fname);
typedef ALCAPI const ALubyte* (ALCAPIENTRY * ALGETSTRING)(ALenum param);
typedef ALCAPI ALboolean (ALCAPIENTRY * ALISEXTENSIONPRESENT)(const ALubyte* ename);
typedef ALCAPI ALboolean (ALCAPIENTRY * ALISENABLED)(ALenum capability);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALLISTENERF)(ALenum param, ALfloat value);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALLISTENER3F)(ALenum param, ALfloat v1, ALfloat v2, ALfloat v3);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALLISTENERFV)(ALenum param, const ALfloat* values);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALLISTENERI)(ALenum param, ALint value);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETLISTENERF)(ALenum param, ALfloat* value);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETLISTENER3F)(ALenum param, ALfloat* v1, ALfloat* v2, ALfloat* v3);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETLISTENERFV)(ALenum param, ALfloat* values);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETLISTENERI)(ALenum param, ALint* value);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGENSOURCES)(ALsizei n, ALuint* sourceNames);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALDELETESOURCES)(ALsizei n, const ALuint* sourceNames);
typedef ALCAPI ALboolean (ALCAPIENTRY * ALISSOURCE)(ALuint sourceName);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCEF)(ALuint sourceName, ALenum param, ALfloat value);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCEFV)(ALuint sourceName, ALenum param, const ALfloat* values);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCE3F)(ALuint sourceName, ALenum param, ALfloat v1, ALfloat v2, ALfloat v3);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCEI)(ALuint sourceName, ALenum param, ALint value);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETSOURCEF)(ALuint sourceName, ALenum param, ALfloat* value);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETSOURCE3F)(ALuint sourceName, ALenum param, ALfloat* v1, ALfloat* v2, ALfloat* v3);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETSOURCEFV)(ALuint sourceName, ALenum param, ALfloat* values);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALGETSOURCEI)(ALuint sourceName, ALenum param, ALint* value);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCEPLAY)(ALuint sourceName);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCEPLAYV)(ALsizei n, const ALuint* sourceNames);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCEPAUSE)(ALuint sourceName);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCEPAUSEV)(ALsizei n, const ALuint* sourceNames);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCESTOP)(ALuint sourceName);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCESTOPV)(ALsizei n, const ALuint* sourceNames);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCEREWIND)(ALuint sourceName);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCEREWINDV)(ALsizei n, const ALuint* sourceNames);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCEQUEUEBUFFERS)(ALuint sourceName, ALsizei n, const ALuint* buffers);
typedef ALCAPI ALvoid (ALCAPIENTRY * ALSOURCEUNQUEUEBUFFERS)(ALuint sourceName, ALsizei n, ALuint* buffers);



extern ALCCLOSEDEVICE			pfalcCloseDevice;
extern ALCCREATECONTEXT		pfalcCreateContext;
extern ALCDESTROYCONTEXT		pfalcDestroyContext;
extern ALCGETCONTEXTSDEVICE	pfalcGetContextsDevice;
extern ALCGETCURRENTCONTEXT	pfalcGetCurrentContext;
extern ALCGETENUMVALUE			pfalcGetEnumValue;
extern ALCGETERROR				pfalcGetError;
extern ALCGETINTEGERV			pfalcGetIntegerv;
extern ALCGETPROCADDRESS		pfalcGetProcAddress;
extern ALCGETSTRING			pfalcGetString;
extern ALCISEXTENSIONPRESENT	pfalcIsExtensionPresent;
extern ALCMAKECONTEXTCURRENT	pfalcMakeContextCurrent;
extern ALCOPENDEVICE			pfalcOpenDevice;
extern ALCPROCESSCONTEXT		pfalcProcessContext;
extern ALCSUSPENDCONTEXT		pfalcSuspendContext;

extern ALGENBUFFERS			pfalGenBuffers;
extern ALDELETEBUFFERS			pfalDeleteBuffers;
extern ALISBUFFER				pfalIsBuffer;
extern ALBUFFERDATA			pfalBufferData;
extern ALGETBUFFERF			pfalGetBufferf;
extern ALGETBUFFERI			pfalGetBufferi;
extern ALENABLE				pfalEnable;
extern ALDISABLE				pfalDisable;
extern ALDOPPLERFACTOR			pfalDopplerFactor;
extern ALDOPPLERVELOCITY		pfalDopplerVelocity;
extern ALDISTANCEMODEL			pfalDistanceModel;
extern ALGETBOOLEAN			pfalGetBoolean;
extern ALGETBOOLEANV			pfalGetBooleanv;
extern ALGETDOUBLE				pfalGetDouble;
extern ALGETDOUBLEV			pfalGetDoublev;
extern ALGETFLOAT				pfalGetFloat;
extern ALGETFLOATV				pfalGetFloatv;
extern ALGETINTEGER			pfalGetInteger;
extern ALGETINTEGERV			pfalGetIntegerv;
extern ALGETENUMVALUE			pfalGetEnumValue;
extern ALGETERROR				pfalGetError;
extern ALGETPROCADDRESS		pfalGetProcAddress;
extern ALGETSTRING				pfalGetString;
extern ALISEXTENSIONPRESENT	pfalIsExtensionPresent;
extern ALISENABLED				pfalIsEnabled;
extern ALLISTENERF				pfalListenerf;
extern ALLISTENER3F			pfalListener3f;
extern ALLISTENERFV			pfalListenerfv;
extern ALLISTENERI				pfalListeneri;
extern ALGETLISTENERF			pfalGetListenerf;
extern ALGETLISTENER3F			pfalGetListener3f;
extern ALGETLISTENERFV			pfalGetListenerfv;
extern ALGETLISTENERI			pfalGetListeneri;
extern ALGENSOURCES			pfalGenSources;
extern ALDELETESOURCES			pfalDeleteSources;
extern ALISSOURCE				pfalIsSource;
extern ALSOURCEF				pfalSourcef;
extern ALSOURCEFV				pfalSourcefv;
extern ALSOURCE3F				pfalSource3f;
extern ALSOURCEI				pfalSourcei;
extern ALGETSOURCEF			pfalGetSourcef;
extern ALGETSOURCE3F			pfalGetSource3f;
extern ALGETSOURCEFV			pfalGetSourcefv;
extern ALGETSOURCEI			pfalGetSourcei;
extern ALSOURCEPLAY			pfalSourcePlay;
extern ALSOURCEPLAYV			pfalSourcePlayv;
extern ALSOURCEPAUSE			pfalSourcePause;
extern ALSOURCEPAUSEV			pfalSourcePausev;
extern ALSOURCESTOP			pfalSourceStop;
extern ALSOURCESTOPV			pfalSourceStopv;
extern ALSOURCEREWIND			pfalSourceRewind;
extern ALSOURCEREWINDV			pfalSourceRewindv;
extern ALSOURCEQUEUEBUFFERS	pfalSourceQueueBuffers;
extern ALSOURCEUNQUEUEBUFFERS	pfalSourceUnqueueBuffers;


#endif /* __OPENAL_BINDING_H__ */

/* EOF */
