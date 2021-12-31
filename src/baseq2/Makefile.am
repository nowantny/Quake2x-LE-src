# $Id: Makefile.am,v 1.8 2002/12/23 14:04:34 jaq Exp $

# Process this file with automake to create Makefile.in

pkglibdir=$(libdir)/@PACKAGE@/baseq2
pkglib_LTLIBRARIES = game.la
game_la_SOURCES = g_ai.c g_chase.c g_cmds.c g_svcmds.c g_combat.c \
		g_func.c g_items.c g_main.c g_misc.c g_monster.c \
		g_phys.c g_save.c g_spawn.c g_target.c g_trigger.c \
		g_turret.c g_utils.c g_weapon.c \
		m_actor.c m_berserk.c m_boss2.c m_boss3.c m_boss31.c \
		m_boss32.c m_brain.c m_chick.c m_flipper.c m_float.c \
		m_flyer.c m_gladiator.c m_gunner.c m_hover.c m_flash.c \
		m_infantry.c m_insane.c m_medic.c m_move.c m_mutant.c \
		m_parasite.c m_soldier.c m_supertank.c m_tank.c \
		p_client.c p_hud.c p_trail.c p_view.c p_weapon.c \
		q_shared.c

noinst_HEADERS = g_local.h m_actor.h m_berserk.h m_boss2.h m_boss31.h \
	m_boss32.h m_brain.h m_chick.h m_flipper.h m_float.h \
	m_flyer.h m_gladiator.h m_gunner.h m_hover.h m_infantry.h \
	m_insane.h m_medic.h m_mutant.h m_parasite.h m_player.h \
	m_rider.h m_soldier.h m_supertank.h m_tank.h


AM_CFLAGS = -fPIC -pipe @WARN_CFLAGS@ @OPT_CFLAGS@
AM_CPPFLAGS = -I$(top_srcdir)/src
LDFLAGS = -module -avoid-version -rpath $(pkglibdir)
