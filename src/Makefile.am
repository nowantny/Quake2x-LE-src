# $Id: Makefile.am,v 1.41 2003/02/06 13:45:39 jaq Exp $

SUBDIRS = . baseq2 ctf xatrix rogue

std_cflags = -pipe @WARN_CFLAGS@ @OPT_CFLAGS@
module_ldflags = -module -avoid-version -rpath $(pkglibdir)

bin_PROGRAMS = quake2
pkglib_LTLIBRARIES = @VID_REFS@
EXTRA_LTLIBRARIES = ref_soft.la ref_softx.la ref_softsdl.la \
		ref_glx.la ref_sdlgl.la ref_tdfx.la

# Common source files
REF_GL_COMMON = q_sh.c q_shared.c q_glob.c qgl.c \
		gl_draw.c gl_image.c gl_light.c gl_mesh.c \
		gl_model.c gl_rmain.c gl_rmisc.c gl_rsurf.c \
		gl_warp.c

REF_SOFT_COMMON = r_aclip.c r_alias.c r_bsp.c r_draw.c r_edge.c \
		r_image.c r_light.c r_main.c r_misc.c r_model.c \
		r_part.c r_poly.c r_polyse.c r_rast.c r_scan.c \
		r_sprite.c r_surf.c \
		q_shared.c q_sh.c q_glob.c

noinst_HEADERS = adivtab.h anorms.h anormtab.h asm_draw.h asm_i386.h block8.h \
	block16.h cdaudio.h client.h console.h crc.h d_ifacea.h game.h \
	gcc_attr.h gl_local.h gl_model.h q_glob.h glw.h input.h keys.h \
	mdfour.h q_shared.h qasm.h qcommon.h qfiles.h qgl.h qmenu.h \
	quakeio.h r_local.h r_model.h rand1k.h ref.h rw.h screen.h \
	server.h snd_loc.h sound.h uint32.h vid.h warpsin.h \
	ref_gl.def ref_gl.dsp ref_soft.def ref_soft.dsp

if ASM_ARCH
REF_SOFT_ASM = r_aclipa.S r_draw16.S r_drawa.S r_edgea.S r_scana.S \
		r_spr8.S r_surf8.S math.S d_polysa.S r_varsa.S \
		sys_dosa.S
else
REF_SOFT_ASM =
endif

if ASM_ARCH
QUAKE2_ASM=snd_mixa.S
else
QUAKE2_ASM=
endif

if BUILD_SDLQUAKE2
SOUND = cd_sdl.c snd_sdl.c snd_dma.c snd_mix.c snd_mem.c
else
SOUND = cd.c snd.c snd_dma.c snd_mix.c snd_mem.c
endif

quake2_SOURCES = main.c q_sh.c vid_menu.c vid_so.c q_glob.c net_udp.c \
		\
		cl_cin.c cl_ents.c cl_fx.c cl_input.c cl_inv.c \
		cl_main.c cl_parse.c cl_pred.c cl_tent.c cl_scrn.c \
		cl_view.c cl_newfx.c console.c keys.c menu.c qmenu.c \
		\
		checksum.c cmd.c cmodel.c common.c crc.c cvar.c \
		files.c mdfour.c net_chan.c \
		\
		sv_ccmds.c sv_ents.c sv_game.c sv_init.c sv_main.c \
		sv_send.c sv_user.c sv_world.c \
		\
		q_shared.c pmove.c \
		\
		m_flash.c \
		\
		$(QUAKE2_ASM) $(SOUND)

# include the asm and so forth when building on non-x86 architectures
EXTRA_ref_soft_la_SOURCES = r_aclipa.S r_draw16.S r_drawa.S r_edgea.S \
				r_scana.S r_spr8.S r_surf8.S math.S \
				d_polysa.S r_varsa.S sys_dosa.S d_copy.S \
				rw_svgalib.c rw_in_svgalib.c

EXTRA_ref_glx_la_SOURCES = gl_glx.c

EXTRA_ref_softsdl_la_SOURCES = rw_sdl.c

EXTRA_ref_tdfx_la_SOURCES =  rw_in_svgalib.c gl_fxmesa.c

EXTRA_ref_softx_la_SOURCES = rw_x11.c

EXTRA_quake2_SOURCES = snd_mixa.S cd_sdl.c snd_sdl.c snd_dma.c \
			snd_mix.c snd_mem.c cd.c snd.c

if BUILD_SDLQUAKE2
quake2_CFLAGS = $(std_cflags) @PTHREAD_CFLAGS@ @SDL_CFLAGS@
else
quake2_CFLAGS = $(std_cflags) @PTHREAD_CFLAGS@
endif
# the macro puts -pthread into cflags, but we want to link
# with this flag too, so stick it into the ldflags if it's there...
# extra cflags at link time can't hurt
quake2_LDFLAGS = @PTHREAD_CFLAGS@
if BUILD_SDLQUAKE2
quake2_LDADD = @PTHREAD_LIBS@ @DL_LIBS@ @OSS_LIBS@ @SDL_LIBS@ @SYSTEM_LIBS@ -lm
else
quake2_LDADD = @PTHREAD_LIBS@ @DL_LIBS@ @OSS_LIBS@ @SYSTEM_LIBS@ -lm
endif

# ref_glx
if BUILD_GLX
ref_glx_la_SOURCES = $(REF_GL_COMMON) gl_glx.c
ref_glx_la_CFLAGS = $(std_cflags) -fPIC @X_CFLAGS@
ref_glx_la_LIBADD = @X_PRE_LIBS@ @X_LIBS@ @XTRA_LIBS@ @X_EXTRA_LIBS@ @DL_LIBS@
ref_glx_la_LDFLAGS = $(module_ldflags)
endif

# ref_tdfx
if BUILD_TDFX
ref_tdfx_la_SOURCES = $(REF_GL_COMMON) rw_in_svgalib.c gl_fxmesa.c
ref_tdfx_la_CFLAGS = $(std_cflags) -fPIC @SVGALIB_CFLAGS@ @OPENGL_CFLAGS@
ref_tdfx_la_LIBADD = @SVGALIB_LIBS@
ref_tdfx_la_LDFLAGS = $(module_ldflags)
endif

# ref_sdlgl
if BUILD_SDLGL
ref_sdlgl_la_SOURCES = $(REF_GL_COMMON) rw_sdl.c
ref_sdlgl_la_CFLAGS = $(std_cflags) -fPIC -DOPENGL @SDL_CFLAGS@ @OPENGL_CFLAGS@
#ref_sdlgl_la_CFLAGS = $(std_cflags) -fPIC -DOPENGL @SDL_CFLAGS@
ref_sdlgl_la_LIBADD = @SDL_LIBS@ @OPENGL_LIBS@
#ref_sdlgl_la_LIBADD = @SDL_LIBS@
ref_sdlgl_la_LDFLAGS = $(module_ldflags)
endif

# ref_soft
if BUILD_SOFT
if ASM_ARCH
DCOPY_S = d_copy.S
else
DCOPY_S =
endif
ref_soft_la_SOURCES = $(REF_SOFT_COMMON) $(REF_SOFT_ASM) $(DCOPY_S) \
	rw_svgalib.c rw_in_svgalib.c
ref_soft_la_CFLAGS = $(std_cflags) -fPIC @SVGALIB_CFLAGS@
ref_soft_la_LIBADD = @SVGALIB_LIBS@
ref_soft_la_LDFLAGS = $(module_ldflags)
endif

# ref_softx
if BUILD_SOFTX
ref_softx_la_SOURCES = $(REF_SOFT_COMMON) $(REF_SOFT_ASM) rw_x11.c
ref_softx_la_CFLAGS = $(std_cflags) -fPIC @X_CFLAGS@
ref_softx_la_LIBADD = @X_PRE_LIBS@ @X_LIBS@ @XTRA_LIBS@ @X_EXTRA_LIBS@
ref_softx_la_LDFLAGS = $(module_ldflags)
endif

# ref_softsdl
if BUILD_SOFTSDL
ref_softsdl_la_SOURCES = $(REF_SOFT_COMMON) $(REF_SOFT_ASM) rw_sdl.c
ref_softsdl_la_CFLAGS = $(std_cflags) -fPIC @SDL_CFLAGS@
ref_softsdl_la_LIBADD = @SDL_LIBS@
ref_softsdl_la_LDFLAGS = $(module_ldflags)
endif
