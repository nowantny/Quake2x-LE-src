/* $Id: snd.c,v 1.13 2002/11/05 12:38:31 jaq Exp $
 *
 * used to be snd_linux.c
 *
 * Copyright (C) 1997-2001 Id Software, Inc.
 * Copyright (c) 2002 The Quakeforge Project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* merged in from snd_irix.c -- jaq */
#ifndef __sgi
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>

/* some have sys/soundcard.h, others have just soundcard.h */
#ifdef HAVE_SYS_SOUNDCARD_H
# include <sys/soundcard.h>
#endif

#ifdef HAVE_SOUNDCARD_H
# include <soundcard.h>
#endif

#else /* __sgi */
#include <dmedia/dmedia.h>
#include <dmedia/audio.h>
#endif /* __sgi */

#include "client.h"
#include "snd_loc.h"

#define FRAGSIZEEXP 9
#define FRAGSIZE    (1<<FRAGSIZEEXP)

#define AUDIOBUFFERSIZE 4096
#define AUDIOBUFFERS    64

#ifdef __sgi

/* must be a power of 2! */
#define QSND_SKID 2
#define QSND_BUFFER_FRAMES 8192
#define QSND_BUFFER_SIZE   (QSND_BUFFER_FRAMES * 2)

#define UST_TO_BUFFPOS(ust) ((int)((ust) & (QSND_BUFFERS_FRAMES - 1)) << 1)

short int dma_buffer[QSND_BUFFER_SIZE];
ALport sgisnd_aport = NULL;
long long sgisnd_startframe;
double sgisnd_frames_per_ns;
long long sgisnd_lastframewritten = 0;

#else /* !__sgi */

static int audio_fd = -1;
static volatile int snd_inited;
static volatile int frags_sent;
static int mmapped = 0;

static int tryrates[] = { 11025, 22051, 44100, 48000, 8000 };

#endif

cvar_t *sndbits;
cvar_t *sndspeed;
cvar_t *sndchannels;
cvar_t *snddevice;
/* irix cvars -- jaq */
cvar_t * s_loadas8bit;
cvar_t * s_khz;

static pthread_t audio;

void * thesound(void * arg) {
	while (snd_inited) {
		write(audio_fd, dma.buffer + frags_sent * FRAGSIZE, FRAGSIZE);
		frags_sent++;
		frags_sent &= (dma.samples * (dma.samplebits/8) / FRAGSIZE) - 1;
	}
	pthread_exit(0L);
/* Not reached */
	return NULL;
}

qboolean SNDDMA_Init(void) {
/* merged in from snd_irix.c -- jaq */
#ifdef __sgi
	ALconfig ac = NULL;
	ALpv pvbuf[2];

	s_loadas8bit = Cvar_Get("s_loadas8bit", "16", CVAR_ARCHIVE);
	if ((int) s_loadas8bit->value)
		dma.samplebits = 8;
	else
		dma.samplebits = 16;

	if (dma.samplebits != 16) {
		Com_Printf("Don't currently support %i-bit data.  Forcing 16-bit\n", dma.samplebits);
		dma.samplebits = 16;
		Cvar_SetValue("s_loadas8bit", false);
	}

	s_khz = Cvar_Get("s_khz", "0", CVAR_ARCHIVE);
	switch ((int) s_khz->value) {
		case 48:
			dma.speed = AL_RATE_48000;
			break;
		case 44:
			dma.speed = AL_RATE_44100;
			break;
		case 32:
			dma.speed = AL_RATE_32000;
			break;
		case 22:
			dma.speed = AL_RATE_22050;
			break;
		case 16:
			dma.speed = AL_RATE_16000;
			break;
		case 11:
			dma.speed = AL_RATE_11025;
			break;
		case 8:
			dma.speed = AL_RATE_8000;
			break;
		default:
			dma.speed = AL_RATE_22050;
			Com_Printf("Don't currently support %ikHz sample rate, using %i.\n", (int) s_khz->value, (int) (dma.speed/1000));
	}

	sndchannels = Cvar_Get("sndchannels", "2", CVAR_ARCHIVE);
	dma.channels = (int) sndchannels->value;
	if (dma.channels != 2)
		Com_Printf("Don't currently support %i sound channels, try 2.\n", dma.channels);

	ac = alNewConfig();
	alSetChannels(ac, AL_STEREO);
	alSetStampFmt(ac, AL_SAMPFMT_TWOSCOMP);
	alSetQueueSize(ac, QSND_BUFFER_FRAMES);
	if (dma.samplebits == 8)
		alSetWidth(ac, AL_SAMPLE_8);
	else
		alSetWidth(ac, AL_SAMPLE_16);

	sgisnd_aport = alOpenPort("Quake", "w", ac);
	if (!sgisnd_aport) {
		printf("failed to open audio port!\n");
	}

	/* set desired sample rate */
	pvbuf[0].param = AL_MASTER_CLOCK;
	pvbuf[0].value.i = AL_CRYSTAL_MCLK_TYPE;
	pvbuf[1].param = AL_RATE;
	pvbuf[1].value.ll = alIntToFixed(dma.speed);
	alSetParams(alGetResource(sgisnd_aport), pvbuf, 2);
	if (pvbuf[1].sizeOut < 0)
		printf("illegal sample rate %d\n", dma.speed);

	sgisnd_frames_per_ns = dma.speed * 1.0e-9;

	dma.samples = sizeof(dma_buffer) / (dma.samplebits / 8);
	dma.submission_chunk = 1;

	dma.buffer = (unsigned char *) dma_buffer;

	dma.samplepos = 0;

	alFreeConfig(ac);
	return true;
#else /* __sgi */
	int rc;
	int fmt;
	int tmp;
	int i;
	struct audio_buf_info info;
	int caps;
	extern uid_t saved_euid;

	if (snd_inited)
		return 1;

	snd_inited = 0;

	if (!snddevice) {
		sndbits = Cvar_Get("sndbits", "16", CVAR_ARCHIVE);
		sndspeed = Cvar_Get("sndspeed", "0", CVAR_ARCHIVE);
		sndchannels = Cvar_Get("sndchannels", "2", CVAR_ARCHIVE);
		snddevice = Cvar_Get("snddevice", "/dev/dsp", CVAR_ARCHIVE);
	}

// open /dev/dsp, check capability to mmap, and get size of dma buffer

/* snd_bsd.c had "if (!audio_fd)" */
	if (audio_fd == -1)
	{
		seteuid(saved_euid);

		audio_fd = open(snddevice->string, O_RDWR);

		/* moved from below in snd_bsd.c -- jaq */
		seteuid(getuid());
		
		if (audio_fd == -1)
		{
			perror(snddevice->string);
			seteuid(getuid());
			Com_Printf("SNDDMA_Init: Could not open %s.\n", snddevice->string);
			return 0;
		}
		/*
		seteuid(getuid());
		*/
	}

    rc = ioctl(audio_fd, SNDCTL_DSP_RESET, 0);
    if (rc == -1) /* snd_bsd has "rc < 0" */
	{
		perror(snddevice->string);
		Com_Printf("SNDDMA_Init: Could not reset %s.\n", snddevice->string);
		close(audio_fd);
		audio_fd = -1;
		return 0;
	}

	if (ioctl(audio_fd, SNDCTL_DSP_GETCAPS, &caps) == -1
			|| !(caps & DSP_CAP_TRIGGER) || !(caps & DSP_CAP_MMAP)) {
		Com_Printf("SNDDMA_Init: Sound device does not support mmap");
	} else {
		if (ioctl(audio_fd, SNDCTL_DSP_GETOSPACE, &info) == -1) {
			perror(snddevice->string);
			Com_Printf("SNDDMA_Init: Sound driver too old.\n");
			close(audio_fd);
			audio_fd = -1;
			return 0;
		} else
			mmapped = 1;
	}

	if (!(caps & DSP_CAP_TRIGGER) || !(caps & DSP_CAP_MMAP)) {
		if (!mmapped) {
			int frags = 2 << 16 | FRAGSIZEEXP;

			if (ioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &frags) == -1) {
				perror(snddevice->string);
				Com_Printf("SNDDMA_Init: Could not set sound fragments");
/* NOMMAP - jaq
		Com_Printf("SNDDMA_Init: Sorry, but your soundcard doesn't support trigger or mmap. (%08x)\n", caps);
*/
				close(audio_fd);
				audio_fd = -1;
				return 0;
			}
		}
	}
/* NOMMAP - jaq
    if (ioctl(audio_fd, SNDCTL_DSP_GETOSPACE, &info)==-1)
    {   
        perror("GETOSPACE");
		Com_Printf("SNDDMA_Init: GETOSPACE ioctl failed.\n");
		close(audio_fd);
		audio_fd = -1;
		return 0;
    }
*/
	
// set sample bits & speed

    dma.samplebits = (int)sndbits->value;
	if (dma.samplebits != 16 && dma.samplebits != 8) {
        ioctl(audio_fd, SNDCTL_DSP_GETFMTS, &fmt);
#ifdef HAVE_AFMT_S16_NE
		if (fmt & AFMT_S16_NE)
#else
        if (fmt & AFMT_S16_LE)
#endif
			dma.samplebits = 16;
        else if (fmt & AFMT_U8) dma.samplebits = 8;
    }
/* in relnev 0.9, from here until the next RELNEV 0.9 comment has been moved
 * down to the following RELNEV 0.9 comment -- jaq */
	dma.speed = (int)sndspeed->value;
	if (!dma.speed) {
		for (i=0 ; i<sizeof(tryrates)/4 ; i++)
			if (!ioctl(audio_fd, SNDCTL_DSP_SPEED, &tryrates[i]))
				break;
		dma.speed = tryrates[i];
	}

	dma.channels = (int)sndchannels->value;
	if (dma.channels < 1 || dma.channels > 2)
		dma.channels = 2;

	tmp = 0;
	if (dma.channels == 2)
		tmp = 1;
    rc = ioctl(audio_fd, SNDCTL_DSP_STEREO, &tmp);
    if (rc < 0)
    {
		perror(snddevice->string);
		Com_Printf("SNDDMA_Init: Could not set %s to stereo=%d.", snddevice->string, dma.channels);
		close(audio_fd);
		audio_fd = -1;
        return 0;
    }

	if (tmp)
		dma.channels = 2;
	else
		dma.channels = 1;
	/* RELNEV 0.9 end deletion */

    if (dma.samplebits == 16)
    {
#ifdef HAVE_AFMT_S16_NE
        rc = AFMT_S16_NE;
#else
		rc = AFMT_S16_LE;
#endif
        rc = ioctl(audio_fd, SNDCTL_DSP_SETFMT, &rc);
        if (rc < 0)
		{
			perror(snddevice->string);
			Com_Printf("SNDDMA_Init: Could not support 16-bit data.  Try 8-bit.\n");
			close(audio_fd);
			audio_fd = -1;
			return 0;
		}
    }
    else if (dma.samplebits == 8)
    {
        rc = AFMT_U8;
        rc = ioctl(audio_fd, SNDCTL_DSP_SETFMT, &rc);
        if (rc < 0)
		{
			perror(snddevice->string);
			Com_Printf("SNDDMA_Init: Could not support 8-bit data.\n");
			close(audio_fd);
			audio_fd = -1;
			return 0;
		}
    }
	else
	{
		perror(snddevice->string);
		Com_Printf("SNDDMA_Init: %d-bit sound not supported.", dma.samplebits);
		close(audio_fd);
		audio_fd = -1;
		return 0;
	}

	/* RELNEV 0.9 insert some here */

	rc = ioctl(audio_fd, SNDCTL_DSP_SPEED, &dma.speed);
	if (rc < 0)
	{
		perror(snddevice->string);
		Com_Printf("SNDDMA_Init: Could not set %s speed to %d.", snddevice->string, dma.speed);
		close(audio_fd);
		audio_fd = -1;
		return 0;
	}

	/* RELNEV 0.9 insert the mmap stuff here */

// toggle the trigger & start her up

	if (!mmapped) {
		dma.submission_chunk = AUDIOBUFFERSIZE;
		dma.samples = AUDIOBUFFERS * dma.submission_chunk * dma.channels;
		dma.samplepos = 0;
		
		if ((dma.buffer = malloc(dma.samples * (dma.samplebits/8))) == 0) {
			perror(snddevice->string);
			Com_Printf("Could not allocate shm->buffer");
			close(audio_fd);
			audio_fd = -1;
			return 0;
		}

		snd_inited = 1;

		pthread_create(&audio, 0L, thesound, 0L);
	} else {
	    tmp = 0;
    	rc  = ioctl(audio_fd, SNDCTL_DSP_SETTRIGGER, &tmp);
		dma.samples = info.fragstotal * info.fragsize / (dma.samplebits/8);
		dma.submission_chunk = 1;

		// memory map the dma buffer

		if (!dma.buffer) {
			dma.buffer = (unsigned char *) mmap(NULL, info.fragstotal * info.fragsize,
#if defined(__FreeBSD__) && (__FreeBSD_version < 500000)
				PROT_READ|PROT_WRITE,
#else
				PROT_WRITE,
#endif
				MAP_FILE|MAP_SHARED, audio_fd, 0);
		}
		if (!dma.buffer || dma.buffer == MAP_FAILED) {
			perror(snddevice->string);
			Com_Printf("SNDDMA_Init: Could not mmap %s.\n", snddevice->string);
			close(audio_fd);
			audio_fd = -1;
			return 0;
		}

		if (rc < 0) {
			perror(snddevice->string);
			Com_Printf("SNDDMA_Init: Could not toggle. (1)\n");
			close(audio_fd);
			audio_fd = -1;
			return 0;
		}
    	tmp = PCM_ENABLE_OUTPUT;
	    rc = ioctl(audio_fd, SNDCTL_DSP_SETTRIGGER, &tmp);
		if (rc < 0) {
			perror(snddevice->string);
			Com_Printf("SNDDMA_Init: Could not toggle. (2)\n");
			close(audio_fd);
			audio_fd = -1;
			return 0;
		}

		dma.samplepos = 0;
		snd_inited = 1;
	}
	return 1;
#endif /* !__sgi */
}

/*
 * SNDDMA_GetDMAPos
 *
 * return the current sample position (in mono samples, not stereo)
 * insde the recirculating dma buffer, so the mixing code will know
 * how many samples are required to fill it up.
 */
int SNDDMA_GetDMAPos(void) {
/* merged in from snd_irix.c -- jaq */
#ifdef __sgi
	long long ustFuture, ustNow;

	if (!sgisnd_aport)
		return 0;

	alGetFrameTime(sgisnd_aport, &sgisnd_startframe, &ustFuture);
	dmGetUST((unsigned long long *) &ustNow);
	sgisnd_startframe -= (long long) ((ustFuture - ustNow) * sgisnd_frames_per_ns);
	sgisnd_startframe += 100;
	/* printf("frame %ld pos %d\n", frame, UST_TO_BUFFPOS(sgisnd_startframe)); */
	return UST_TO_BUFFPOS(sgisnd_startframe);
#else /* __sgi */
	struct count_info count;

	if (!snd_inited) return 0;

	if (!mmapped)
		return (frags_sent * FRAGSIZE) / (dma.samplebits/8) + FRAGSIZE/16;

	if (ioctl(audio_fd, SNDCTL_DSP_GETOPTR, &count)==-1)
	{
		perror(snddevice->string);
		Com_Printf("SNDDMA_GetDMAPos: GETOPTR failed.\n");
		close(audio_fd);
		audio_fd = -1;
		snd_inited = 0;
		return 0;
	}
//	dma.samplepos = (count.bytes / (dma.samplebits / 8)) & (dma.samples-1);
//	fprintf(stderr, "%d    \r", count.ptr);
	dma.samplepos = count.ptr / (dma.samplebits / 8);

	return dma.samplepos;
#endif /* __sgi */
}

/*
 * SNDDMA_Shutdown
 * Reset the sound device for exiting
 */
void SNDDMA_Shutdown(void) {
	printf ("SNDDMA_Shutdown\n");
/* merged in from snd_irix.c -- jaq */
#ifdef __sgi
	if (sgisnd_aport) {
		alClosePort(sgisnd_aport);
		sgisnd_aport = NULL;
	}
#else
	if (snd_inited) {
		if (!mmapped) {
			snd_inited = 0L;
			pthread_join(audio, 0L);

			if (dma.buffer) {
				free(dma.buffer);
				dma.buffer = 0L;
			}
		} else {
			munmap (dma.buffer, dma.samples *dma.samplebits / 8);
			dma.buffer = 0L;
		}

		close(audio_fd);
		audio_fd = -1;
		snd_inited = 0;
	}
#endif
}

/*
==============
SNDDMA_Submit

Send sound to device if buffer isn't really the dma buffer
===============
*/

/* merged in from snd_irix.c -- jaq */
#ifdef __sgi
extern int soundtime;
#endif

void SNDDMA_Submit(void) {
#ifdef __sgi
	int nFillable, nFilled, nPos;
	int nFrames, nFramesLeft;
	unsigned endtime;

	if (!sgisnd_aport)
		return;

	nFillable = alGetFillable(sgisnd_aport);
	nFilled = QSND_BUFFER_FRAMES - nFillable;

	nFrames = dma.samples >> (dma.channels - 1);

	if (paintedtime - soundtime < nFrames)
		nFrames = paintedtime - soundtime;

	if (nFrames <= QSND_SKID)
		return;

	nPos = UST_TO_BUFFPOS(sgisnd_startframe);

	/* dump rewritten contents of the buffer */
	if (sgisnd_lastframewritten > sgisnd_startframe) {
		alDiscardFrames(sgisnd_aport, sgisnd_lastframewritten - sgisnd_startframe);
	} else if ((int) (sgisnd_startframe - sgisnd_lastframewritten) >= QSND_BUFFER_FRAMES) {
		/* blow away everything if we've underflowed */
		alDiscardFrames(sgisnd_aport, QSND_BUFFER_FRAMES);
	}

	/* don't block */
	if (nFrames > nFillable)
		nFrames = nFillable;

	/* account for stereo */
	nFramesLeft = nFrames;
	if (nPos + nFrames * dma.channels > QSND_BUFFER_SIZE) {
		int nFramesAtEnd = (QSND_BUFFER_SIZE - nPos) >> (dma.channels - 1);

		alWriteFrames(sgisnd_aport, &dma_buffer[nPos], nFramesAtEnd);
		nPos = 0;
		nFramesLeft -= nFramesAtEnd;
	}
	alWriteFrames(sgi_aport, &dma_buffer[nPos], nFramesLeft);

	sgisnd_lastframewritten = sgisnd_startframe + nFrames;
#endif /* __sgi */
}

void SNDDMA_BeginPainting (void)
{
}

