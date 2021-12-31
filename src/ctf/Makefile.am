# $Id: Makefile.am,v 1.7 2002/12/23 14:04:34 jaq Exp $

pkglibdir=$(libdir)/@PACKAGE@/ctf
pkglib_LTLIBRARIES = game.la
game_la_SOURCES = g_ai.c g_chase.c g_cmds.c g_combat.c g_ctf.c \
		g_func.c g_items.c g_main.c g_misc.c g_monster.c \
		g_phys.c g_save.c g_spawn.c g_svcmds.c g_target.c \
		g_trigger.c g_utils.c g_weapon.c \
		m_move.c \
		p_client.c p_hud.c p_menu.c p_trail.c p_view.c \
		p_weapon.c \
		q_shared.c

noinst_HEADERS = g_ctf.h g_local.h m_player.h p_menu.h

AM_CFLAGS = -fPIC -pipe @WARN_CFLAGS@ @OPT_CFLAGS@
AM_CPPFLAGS = -I$(top_srcdir)/src
LDFLAGS = -module -avoid-version -rpath $(pkglibdir)
