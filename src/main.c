/* $Id: main.c,v 1.19 2003/01/11 05:18:59 jaq Exp $
 *
 * used to be sys_linux.c
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

#ifdef _XBOX
#include <xtl.h>
#endif

#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* merged from sys_*.c -- jaq */
#if defined(__linux__) || defined(__sgi)
	#include <mntent.h>
#elif defined(__FreeBSD__) || defined(__bsd__) || defined (__NetBSD__)
	#include <fstab.h>
#elif defined(__sun__)
	#include <sys/file.h>
#endif

#ifdef HAVE_DLOPEN
# include <dlfcn.h>
#endif

#include "qcommon.h"
#include "game.h"
#include "rw.h"

static char	console_text[256];
static int	console_textlen;

#ifdef SOL8_XIL_WORKAROUND
#include <xil/xil.h>
typedef struct {
    qboolean restart_sound;
    int s_rate;
    int s_width;
    int s_channels;

    int width;
    int height;
    byte * pic;
    byte * pic_pending;

    /* order 1 huffman stuff */
    int * hnodes1; /* [256][256][2] */
    int numhnodes1[256];
    int h_used[512];
    int h_count[512];
} cinematics_t;
#endif
    

cvar_t *nostdout;

unsigned	sys_frame_time;

uid_t saved_euid;
qboolean stdin_active = true;
char display_name[1024];
/* FIXME: replace with configure test for hrtime_t */
#ifdef __sun__
hrtime_t base_hrtime;
#endif

int		modmenu_argc = 0;
char	**modmenu_argv = NULL;

// =======================================================================
// General routines
// =======================================================================

void Sys_ConsoleOutput (char *string)
{
	if (nostdout && nostdout->value)
		return;

	fputs(string, stdout);
}

void Sys_Printf (char *fmt, ...) {
	va_list argptr;
	char text[1024];
	unsigned char * p;

	va_start (argptr,fmt);
	vsnprintf (text,1024,fmt,argptr);
	va_end (argptr);

	/* relnev 0.9 deleted -- jaq */
	/* if (strlen(text) > sizeof(text))
		Sys_Error("memory overwrite in Sys_Printf"); */

    if (nostdout && nostdout->value)
        return;

	for (p = (unsigned char *)text; *p; p++) {
		*p &= 0x7f;
		if ((*p > 128 || *p < 32) && *p != 10 && *p != 13 && *p != 9)
			printf("[%02x]", *p);
		else
			putc(*p, stdout);
	}
}

void Sys_Quit (void) {
	 
	CL_Shutdown();
	Qcommon_Shutdown (); 
	 
	XLaunchNewImage("D:\\default.xbe", NULL);
}

void Sys_Init(void)
{
/*#ifdef USE_ASM
//	Sys_SetFPCW();
#endif
*/
}

void Sys_Error (char *error, ...)
{ 
	va_list		argptr;
	char		text[1024];

	CL_Shutdown ();
	Qcommon_Shutdown ();

	va_start (argptr, error);
	vsprintf (text, error, argptr);
	va_end (argptr);

	 
 
	XLaunchNewImage("D:\\default.xbe", NULL);
} 

void Sys_Warn (char *warning, ...)
{ 
    va_list     argptr;
    char        string[1024];
    
    va_start (argptr,warning);
    vsnprintf (string,1024,warning,argptr);
    va_end (argptr);
	fprintf(stderr, "Warning: %s", string);
} 

/*
============
Sys_FileTime

returns -1 if not present
============
*/
int	Sys_FileTime (char *path)
{
	struct	stat	buf;
	
	if (stat (path,&buf) == -1)
		return -1;
	
	return buf.st_mtime;
}

void floating_point_exception_handler(int whatever)
{
//	Sys_Warn("floating point exception\n");
	signal(SIGFPE, floating_point_exception_handler);
}

char *Sys_ConsoleInput (void)
{
 
	return 0;
}

/*****************************************************************************/

static void * game_library = NULL;
typedef game_export_t * gameapi_t(game_import_t *);

void Sys_UnloadGame (void)
{
#ifndef GAME_HARD_LINKED
	if (game_library) 
		dlclose (game_library);
#endif
	game_library = NULL;
}
 
void *Sys_GetGameAPI (void *parms)
{
#ifndef GAME_HARD_LINKED
	void	*(*GetGameAPI) (void *);
#endif

	char	name[MAX_OSPATH];
	char	curpath[MAX_OSPATH];
	char	*path;
#ifdef __i386__
	const char *gamename = "gamei386.so";
#elif defined __alpha__
	const char *gamename = "gameaxp.so";
#elif defined __sh__
	const char *gamename = "gamesh.so";
#else
#error Unknown arch
#endif

//	setreuid(getuid(), getuid());
//	setegid(getgid());

	if (game_library)
		Com_Error (ERR_FATAL, "Sys_GetGameAPI without Sys_UnloadingGame");

//	getcwd(curpath, sizeof(curpath));
//	strcpy(curpath,"/pc");

#ifndef GAME_HARD_LINKED
	Com_Printf("------- Loading %s -------\n", gamename);

	// now run through the search paths
	path = NULL;
	while (1)
	{
		path = FS_NextPath (path);
		if (!path)
			return NULL;		// couldn't find one anywhere
		sprintf (name, "%s/%s", path, gamename);
		printf("load:%s\n",name);
		game_library = dlopen (name, RTLD_LAZY );
		if (game_library)
		{
			Com_Printf ("LoadLibrary (%s)\n",name);
			break;
		}
	}

	GetGameAPI = (void *)dlsym (game_library, "GetGameAPI");
	if (!GetGameAPI)
	{
		Sys_UnloadGame ();		
		return NULL;
	}
#endif

	return (void *)GetGameAPI (parms);
}

/*****************************************************************************/

void Sys_AppActivate (void)
{
}

void Sys_SendKeyEvents (void)
{
#ifndef DEDICATED_ONLY
	 
	KBD_Update();
#endif

	// grab frame time 
	sys_frame_time = Sys_Milliseconds();
}

/*****************************************************************************/
#ifdef SOL8_XIL_WORKAROUND
XilSystemState xil_state;
#endif

int main (int argc, char **argv)
{
	int 	time, oldtime, newtime;

	const static char *basedirs[]={
	"/cd/quake2", /* installed or shareware */
	"/cd/install/data", /* official CD-ROM */
	"/pc/quake2", /* debug */
	NULL
	};

	char *basedir;
	int i;

	int		n, d = 0, slcl;


	FILE *fp = NULL;

	char szCmdLine[80];
	char commandLine[1024];

	XSetFileCacheSize(0x80000);

	fp = fopen("T:\\quake2.params", "rb");
	if (fp)
	{
		fread(commandLine, 1024, 1, fp);
		fclose(fp);
		
		// Count the arguments
		modmenu_argc = 1;
		for(n = 0; n < strlen(commandLine); n++)
		{
			if (commandLine[n] == ' ')
				 modmenu_argc++;
		}

		// Set up modmenu_argv
		modmenu_argv = (char **)malloc(sizeof(char **) * modmenu_argc);
		modmenu_argv[0] = commandLine;
		d = 1;
		slcl = strlen(commandLine);
		for(n = 0; n < slcl; n++)
		{
			if(commandLine[n] == ' ')
			{
				commandLine[n] = 0;
				modmenu_argv[d++] = commandLine + n + 1;
			}
		}

 	}
	
	/*for(i=0;(basedir = basedirs[i])!=NULL;i++) {
		FILE *fd  = fs_open(basedir,O_DIR);
		if (fd!=0) {
			fs_close(fd);
			break;
		}
	} */
	basedir = "D:\\baseq2";

	if (basedir==NULL)
		Sys_Error("can't find quake dir");

//	static char *args[] = {"quake2","-basedir","/pc/quake2",NULL,NULL};
//	argv = args;
//	argc = 4;
//	extern cvar_t	*fs_basedir;

	// go back to real user for config loads
//	saved_euid = geteuid();
//	seteuid(getuid());
	//Cvar_Set("basedir",basedir);

	Qcommon_Init(modmenu_argc, modmenu_argv);

//	fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) | FNDELAY);

	nostdout = Cvar_Get("nostdout", "0", 0);
	if (!nostdout->value) {
//		fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) | FNDELAY);
//		printf ("Linux Quake -- Version %0.3f\n", LINUX_VERSION);
	}

//	printf("basedir=%s\n",fs_basedir->string);

    oldtime = Sys_Milliseconds ();
    while (1)
    {
// find time spent rendering last frame
		do {
			newtime = Sys_Milliseconds ();
			time = newtime - oldtime;
		} while (time < 1);
        Qcommon_Frame (time);
		oldtime = newtime;
    }

}

#if 0
void Sys_CopyProtect(void)
{
/* merged in from sys_bsd.c -- jaq */
#ifdef __linux__
	FILE *mnt;
	struct mntent *ent;
#else /* __bsd__ */
	struct fstab * ent;
#endif
	char path[MAX_OSPATH];
	struct stat st;
	qboolean found_cd = false;

	static qboolean checked = false;

	if (checked)
		return;

	/* sys_irix.c -- jaq
	Com_Printf("XXX - Sys_CopyProtect disabled\n");
	checked = true;
	return;
	*/

/* merged in from sys_bsd.c */
#ifdef __linux__
	if ((mnt = setmntent("/etc/mtab", "r")) == NULL)
		Com_Error(ERR_FATAL, "Can't read mount table to determine mounted cd location.");

	while ((ent = getmntent(mnt)) != NULL) {
		if (strcmp(ent->mnt_type, "iso9660") == 0) {
#else /* __bsd__ */
	while ((ent = getfsent()) != NULL) {
		if (strcmp(ent->fs_vfstype, "cd9660") == 0) {
#endif
			// found a cd file system
			found_cd = true;
/* merged in from sys_bsd.c */
#ifdef __linux__
			sprintf(path, "%s/%s", ent->mnt_dir, "install/data/quake2.exe");
#else /* __bsd__ */
			sprintf(path, "%s/%s", ent->fs_file, "install/data/quake2.exe");
#endif
			if (stat(path, &st) == 0) {
				// found it
				checked = true;
/* merged in from sys_bsd.c */
#ifdef __linux__
				endmntent(mnt);
#else /* __bsd__ */
				endfsent();
#endif
				return;
			}
/* merged in from sys_bsd.c */
#ifdef __linux__
			sprintf(path, "%s/%s", ent->mnt_dir, "Install/Data/quake2.exe");
#else /* __bsd__ */
			sprintf(path, "%s/%s", ent->fs_file, "Install/Data/quake2.exe");
#endif
			
			if (stat(path, &st) == 0) {
				// found it
				checked = true;
/* merged in from sys_bsd.c */
#ifdef __linux__
				endmntent(mnt);
#else /* __bsd__ */
				endfsent();
#endif
				return;
			}
/* merged in from sys_bsd.c */
#ifdef __linux__
			sprintf(path, "%s/%s", ent->mnt_dir, "quake2.exe");
#else /* __bsd__ */
			sprintf(path, "%s/%s", ent->fs_file, "quake2.exe");
#endif
			if (stat(path, &st) == 0) {
				// found it
				checked = true;
/* merged in from sys_bsd.c */
#ifdef __linux__
				endmntent(mnt);
#else /* __bsd__ */
				endfsent();
#endif
				return;
			}
		}
	}
/* merged in from sys_bsd.c */
#ifdef __linux__
	endmntent(mnt);
#else /* __bsd__ */
	endfsent();
#endif

	if (found_cd)
		Com_Error (ERR_FATAL, "Could not find a Quake2 CD in your CD drive.");
	Com_Error (ERR_FATAL, "Unable to find a mounted iso9660 file system.\n"
		"You must mount the Quake2 CD in a cdrom drive in order to play.");
}
#endif

#if 0
/*
================
Sys_MakeCodeWriteable
================
*/
void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{

	int r;
	unsigned long addr;
	int psize = getpagesize();

	addr = (startaddr & ~(psize-1)) - psize;

//	fprintf(stderr, "writable code %lx(%lx)-%lx, length=%lx\n", startaddr,
//			addr, startaddr+length, length);

	r = mprotect((char*)addr, length + startaddr - addr + psize, 7);

	if (r < 0)
    		Sys_Error("Protection change failed\n");

}

#endif

size_t verify_fread(void * ptr, size_t size, size_t nitems, FILE * fp) {
    size_t ret;
    int err;

    clearerr(fp);
    ret = fread(ptr, size, nitems, fp);
    err = errno;
    if (ret != nitems) {
	printf("verify_fread(...,%d,%d,...): return value: %d\n", size, nitems, ret);
	if (ret == 0 && ferror(fp)) {
	    printf("   error: %s\n", strerror(err));
	    printf("   fileno=%d\n", fileno(fp));
	}
	/* sleep(5); */
    }
    return ret;
}

size_t verify_fwrite(void * ptr, size_t size, size_t nitems, FILE * fp) {
    size_t ret;
    int err;

    clearerr(fp);
    ret = fwrite(ptr, size, nitems, fp);
    err = errno;
    if (ret != nitems) {
	printf("verify_fwrite(...,%d,%d,...) = %d\n", size, nitems, ret);
	if (ret == 0 && ferror(fp)) {
	    printf("   error: %s\n", strerror(err));
	    printf("   fileno=%d\n", fileno(fp));
	}
    }
    /* sleep(5); */
    return ret;
}
