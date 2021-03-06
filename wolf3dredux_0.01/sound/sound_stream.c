/*
 *	Copyright (C) 2005 Michael Liebscher
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
 *	sound_stream.c:	Sound Stream manager.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	Portion of this code was derived from Quake II Evolved.
 *
 */

#include <string.h>

/* silences some warnings in the vorbisfile header: */
#ifndef OV_EXCLUDE_STATIC_CALLBACKS
# define OV_EXCLUDE_STATIC_CALLBACKS 1
#endif /* !OV_EXCLUDE_STATIC_CALLBACKS */

#include <vorbis/vorbisfile.h>


#include "../common/common.h"
#include "../client/client.h"
#include "../string/com_string.h"

#include "sound_local.h"
#include "sound.h"


typedef struct
{
	char		introName[MAX_GAMEPATH];
	char		loopName[MAX_GAMEPATH];
	_boolean	looping;

	filehandle_t *hFile;


	int			start;
	int			rate;
	unsigned	format;
	void		*vorbisFile;

} musicTrack_t;



#define BUFFER_SIZE		16384

PRIVATE musicTrack_t	bgTrack;

PRIVATE channel_t	*s_streamingChannel;


extern void Sound_StopBGTrack(void);



/*
-----------------------------------------------------------------------------
 Function: ovc_read -OGG read Callback. Reads data from a stream.

 Parameters:
		ptr -[in] Storage location for data.
		size -[in] Item size in bytes.
		nmemb -[in] Maximum number of items to be read.
		datasource -[in] music track data structure.

 Returns: Nothing

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE size_t ovc_read(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	musicTrack_t	*track = (musicTrack_t *)datasource;


	if (! size || ! nmemb) {
		return 0;
	}


	return (size_t)(FS_ReadFile(ptr, size, nmemb, track->hFile));
}

/*
-----------------------------------------------------------------------------
 Function: ovc_read -OGG seek Callback. Moves the file pointer to a specified
					location.

 Parameters:
			datasource -[in] music track data structure.
			offset -[in] Number of bytes from whence.
			whence -[in] Initial position.


 Returns:
		If successful, fseek returns 0. Otherwise, it returns a nonzero
		value.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE int ovc_seek(void *datasource, ogg_int64_t offset, int whence)
{
	musicTrack_t	*track = (musicTrack_t *)datasource;

#ifdef __i386__
	/* needs an extra cast: */
	return (int)(FS_FileSeek(track->hFile, (SW32)offset, (W32)whence));
#else
	return (int)(FS_FileSeek(track->hFile, offset, (W32)whence));
#endif /* __i386__ */
}

/*
-----------------------------------------------------------------------------
 Function: ovc_close -OGG close Callback. Closes a stream.

 Parameters: datasource -[in] music track data structure. (unused)

 Returns: 0 if the stream is successfully closed, otherwise nonzero.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE int ovc_close(void *datasource)
{
	return 0;
}

/*
-----------------------------------------------------------------------------
 Function: ovc_tell -OGG tell Callback. Gets the current position of a file
					pointer.

 Parameters: datasource -[in] music track data structure.

 Returns: The current file position.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE long ovc_tell(void *datasource)
{
	musicTrack_t	*track = (musicTrack_t *)datasource;

	return FS_FileTell(track->hFile);
}

/*
-----------------------------------------------------------------------------
 Function: Sound_OpenBGTrack -OGG read Callback.

 Parameters:
			name -[in] File name to open.
			track -[in/out] Music track data structure.

 Returns: False on error, otherwise true.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE _boolean Sound_OpenBGTrack(const char *name, musicTrack_t *track)
{
	OggVorbis_File	*vorbisFile;
	vorbis_info		*vorbisInfo;
	ov_callbacks	vorbisCallbacks = {ovc_read, ovc_seek, ovc_close, ovc_tell};
	int ret;

	track->hFile = FS_OpenFile(name, (W32)0);
	if (! track->hFile) {
		return false;
	}


	track->vorbisFile = vorbisFile = Z_Malloc(sizeof(OggVorbis_File));

	if ((ret = ov_open_callbacks(track, vorbisFile,
								 NULL, (long)0, vorbisCallbacks)) < 0) {
		switch (ret) {
			case OV_EREAD:
				Com_DPrintf("Sound_OpenBGTrack(): A read from media returned an error (OV_EREAD).(%s)\n",
							name);
				break;
			case OV_ENOTVORBIS:
				Com_DPrintf("Sound_OpenBGTrack(): Bitstream is not Vorbis data (OV_ENOTVORBIS).(%s)\n",
							name);
				break;
			case OV_EVERSION:
				Com_DPrintf("Sound_OpenBGTrack(): Vorbis version mismatch (OV_EVERSION).(%s)\n",
							name);
				break;
			case OV_EBADHEADER:
				Com_DPrintf("Sound_OpenBGTrack(): Invalid Vorbis bitstream header (OV_EBADHEADER).(%s)\n",
							name);
				break;
			case OV_EFAULT:
				Com_DPrintf("Sound_OpenBGTrack(): Internal logic fault; indicates a bug or heap/stack corruption (OV_EFAULT).(%s)\n",
							name);
				break;

		}
		Com_DPrintf("Sound_OpenBGTrack(): Could not open OGG stream (%s)\n",
					name);

		return false;
	}

	vorbisInfo = ov_info(vorbisFile, -1);
	if ((vorbisInfo->channels != 1) && (vorbisInfo->channels != 2)) {
		Com_DPrintf("Sound_OpenBGTrack(): Only mono and stereo OGG files supported (%s)\n",
					name);

		return false;
	}

	track->start = (int)ov_raw_tell(vorbisFile);
	track->rate = (int)vorbisInfo->rate;
	track->format = (unsigned int)((vorbisInfo->channels == 2) ?
								   AL_FORMAT_STEREO16 : AL_FORMAT_MONO16);

	return true;
}

/*
-----------------------------------------------------------------------------
 Function: Sound_CloseBGTrack -Close out background music track.

 Parameters: track -[in] Music track to close.

 Returns: Nothing

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void Sound_CloseBGTrack(musicTrack_t *track)
{
	if (track->vorbisFile) {
		ov_clear(track->vorbisFile);

		Z_Free(track->vorbisFile);
		track->vorbisFile = NULL;
	}

	if (track->hFile) {
		FS_CloseFile(track->hFile);
	}
}

/*
-----------------------------------------------------------------------------
 Function: Sound_StreamBGTrack -Called each frame to update streaming music
								track.

 Parameters: Nothing

 Returns: Nothing

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void Sound_StreamBGTrack(void)
{

	W8		data[BUFFER_SIZE];
	int			processed, queued, state;
	int			size, read, dummy;
	unsigned	buffer;

	dummy = 0;

	if (! s_musicVolume->value) {
		return;
	}

	if (! s_streamingChannel) {
		return;
	}

	/* Unqueue and delete any processed buffers */
	pfalGetSourcei(s_streamingChannel->sourceName, AL_BUFFERS_PROCESSED,
				   &processed);
	if (processed > 0) {
		while (processed--) {
			pfalSourceUnqueueBuffers(s_streamingChannel->sourceName, 1,
									 &buffer);
			pfalDeleteBuffers(1, &buffer);
		}
	}

	/* Make sure we always have at least 4 buffers in the queue */
	pfalGetSourcei(s_streamingChannel->sourceName, AL_BUFFERS_QUEUED, &queued);
	while (queued < 4) {
		size = 0;

		/* Stream from disk */
		while (size < BUFFER_SIZE) {
			read = (int)ov_read(bgTrack.vorbisFile, (char *)(data + size),
								(BUFFER_SIZE - size), 0, 2, 1, &dummy);
			if (read == 0) {
				/* End of file */
				if (! bgTrack.looping) {
					/* Close the intro track */
					Sound_CloseBGTrack(&bgTrack);

					/* Open the loop track */
					if (! Sound_OpenBGTrack(bgTrack.loopName, &bgTrack)) {
						Sound_StopBGTrack();
						return;
					}

					bgTrack.looping = true;
				}

				/* Restart the track, skipping over the header */
				ov_raw_seek(bgTrack.vorbisFile, (ogg_int64_t)bgTrack.start);

				/* Try streaming again */
				read = (int)ov_read(bgTrack.vorbisFile, (char *)(data + size),
									(BUFFER_SIZE - size), 0, 2, 1, &dummy);
			}

			if (read <= 0) {
				/* An error occurred */
				Sound_StopBGTrack();
				return;
			}

			size += read;
		}

		/* Upload and queue the new buffer */
		pfalGenBuffers(1, &buffer);
		pfalBufferData(buffer, (ALenum)bgTrack.format, data, (ALsizei)size,
					   (ALsizei)bgTrack.rate);
		pfalSourceQueueBuffers(s_streamingChannel->sourceName, 1, &buffer);

		queued++;
	}

	/* Update volume */
	pfalSourcef(s_streamingChannel->sourceName, AL_GAIN, s_musicVolume->value);

	/* If not playing, then do so */
	pfalGetSourcei(s_streamingChannel->sourceName, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING) {
		pfalSourcePlay(s_streamingChannel->sourceName);
	}
}

/*
-----------------------------------------------------------------------------
 Function: Sound_StartStreaming -Start streaming background music track.

 Parameters: Nothing

 Returns: Nothing

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void Sound_StartStreaming(void)
{
	if (! sound_initialized) {
		return;
	}

	if (s_streamingChannel) {
		return;
	}

	s_streamingChannel = Sound_PickChannel((W32)0, (W32)0);
	if (! s_streamingChannel) {
		return;
	}

	s_streamingChannel->streaming = true;

	/* hmmm... */
	pfalDeleteSources(1, &s_streamingChannel->sourceName);
	pfalGenSources(1, &s_streamingChannel->sourceName);

	/* Set up the source */
	pfalSourcei(s_streamingChannel->sourceName, AL_BUFFER, 0);
	pfalSourcei(s_streamingChannel->sourceName, AL_LOOPING, AL_FALSE);
	pfalSourcei(s_streamingChannel->sourceName, AL_SOURCE_RELATIVE, AL_TRUE);
	pfalSourcefv(s_streamingChannel->sourceName, AL_POSITION, vec3_origin);
	pfalSourcefv(s_streamingChannel->sourceName, AL_VELOCITY, vec3_origin);
	pfalSourcef((ALuint)s_streamingChannel->sourceName,
				(ALenum)AL_REFERENCE_DISTANCE, (ALfloat)1.0);
	pfalSourcef((ALuint)s_streamingChannel->sourceName,
				(ALenum)AL_MAX_DISTANCE, (ALfloat)1.0);
	pfalSourcef((ALuint)s_streamingChannel->sourceName,
				(ALenum)AL_ROLLOFF_FACTOR, (ALfloat)0.0);
}

/*
-----------------------------------------------------------------------------
 Function: Sound_StopStreaming -Stop playing streaming music track.

 Parameters: Nothing

 Returns: Nothing

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void Sound_StopStreaming(void)
{
	int			processed;
	unsigned	buffer;

	if (! sound_initialized) {
		return;
	}

	if (! s_streamingChannel) {
		return;
	}

	s_streamingChannel->streaming = false;


	pfalSourceStop(s_streamingChannel->sourceName);

	pfalGetSourcei(s_streamingChannel->sourceName, AL_BUFFERS_PROCESSED,
				   &processed);
	if (processed > 0) {
		while (processed--) {
			pfalSourceUnqueueBuffers(s_streamingChannel->sourceName, 1,
									 &buffer);
			pfalDeleteBuffers(1, &buffer);
		}
	}

	pfalSourcei(s_streamingChannel->sourceName, AL_BUFFER, 0);

	/* hmmm... */
	pfalDeleteSources(1, &s_streamingChannel->sourceName);
	pfalGenSources(1, &s_streamingChannel->sourceName);

	s_streamingChannel = NULL;
}

/*
-----------------------------------------------------------------------------
 Function: Sound_StartBGTrack -Play background music track.

 Parameters:
			introTrack -[in] File name of intro track.
			loopTrack -[in] File name of loop track.

 Returns: Nothing

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void Sound_StartBGTrack(const char *introTrack, const char *loopTrack)
{
	if (! sound_initialized) {
		return;
	}

	Sound_StopBGTrack();


	my_strlcpy(bgTrack.introName, introTrack, sizeof(bgTrack.introName));
	my_strlcpy(bgTrack.loopName, loopTrack, sizeof(bgTrack.loopName));

	Sound_StartStreaming();

	if (! Sound_OpenBGTrack(bgTrack.introName, &bgTrack)) {
		Sound_StopBGTrack();
		return;
	}

	Sound_StreamBGTrack();
}

/*
-----------------------------------------------------------------------------
 Function: Sound_StopBGTrack -Stop playing background track.

 Parameters: Nothing

 Returns: Nothing

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void Sound_StopBGTrack(void)
{
	if (! sound_initialized) {
		return;
	}

	Sound_StopStreaming();

	Sound_CloseBGTrack(&bgTrack);

	memset(&bgTrack, 0, sizeof(musicTrack_t));
}

/* EOF */
