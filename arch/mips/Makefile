# $Id: Makefile,v 1.17 1999/05/02 20:56:18 harald Exp $
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1994, 1995, 1996 by Ralf Baechle
# DECStation modifications by Paul M. Antoine, 1996
#
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies. Remember to do have actions
# for "archclean" and "archdep" for cleaning up and making dependencies for
# this architecture
#

#
# Select the object file format to substitute into the linker script.
#
ifdef CONFIG_CPU_LITTLE_ENDIAN
ifdef CONFIG_CPU_R5900
tool-prefix	= mipsEEel-linux-
else
tool-prefix	= mipsel-linux-
endif
ifdef CONFIG_MIPS_ECOFF
oformat		= ecoff-littlemips
else
oformat		= elf32-littlemips
endif
else
tool-prefix	= mips-linux-
ifdef CONFIG_MIPS_ECOFF
oformat		= ecoff-bigmips
else
oformat		= elf32-bigmips
endif
endif

ifdef CONFIG_CROSSCOMPILE
CROSS_COMPILE  = $(tool-prefix)
endif

LINKFLAGS	= -static
LD_RFLAG	=
MODFLAGS	+= -mlong-calls

#
# The new ELF GCC uses -G0 -mabicalls -fpic as default.  We don't need PIC
# code in the kernel since it only slows down the whole thing.  For the
# old GCC these options are just the defaults.  At some point we might
# make use of global pointer optimizations.
#
# The DECStation requires an ECOFF kernel for remote booting, other MIPS
# machines may also.
#
ifdef CONFIG_ELF_KERNEL
CFLAGS		+= -G 0 -mno-abicalls -fno-pic
LINKFLAGS	+= -G 0
LD_RFLAG	+= -G 0
endif
ifdef CONFIG_ECOFF_KERNEL
CFLAGS		+= -G 0 -mno-abicalls -fno-pic
LINKFLAGS	+= -G 0 -oformat ecoff-littlemips
LD_RFLAG	+= -G 0
endif

ifdef CONFIG_REMOTE_DEBUG
CFLAGS		:= $(CFLAGS) -g
endif

#
# CPU-dependent compiler/assembler options for optimization.
#
ifdef CONFIG_CPU_R3000
CFLAGS		:= $(CFLAGS) -mcpu=r3000 -mips1
endif
ifdef CONFIG_CPU_R6000
CFLAGS		:= $(CFLAGS) -mcpu=r6000 -mips2
endif
ifdef CONFIG_CPU_R4300
CFLAGS		:= $(CFLAGS) -mcpu=r4300 -mips2
endif
ifdef CONFIG_CPU_R4X00
CFLAGS		:= $(CFLAGS) -mcpu=r4600 -mips2
endif
ifdef CONFIG_CPU_R5000
CFLAGS		:= $(CFLAGS) -mcpu=r8000 -mips2
endif
ifdef CONFIG_CPU_NEVADA
CFLAGS		:= $(CFLAGS) -mcpu=r8000 -mips2 -mmad
endif
ifdef CONFIG_CPU_R5900
CFLAGS		:= $(CFLAGS) -mcpu=r5900 -mips1
endif
ifdef CONFIG_CPU_R8000
CFLAGS		:= $(CFLAGS) -mcpu=r8000 -mips2
endif
ifdef CONFIG_CPU_R10000
CFLAGS		:= $(CFLAGS) -mcpu=r8000 -mips2
endif

#
# Board-dependent options and extra files
#
ifdef CONFIG_ALGOR_P4032
CORE_FILES    += arch/mips/algor/algor.o
SUBDIRS       += arch/mips/algor
#LOADADDR      += 0x80000000
endif

#
# DECstation family
#
ifdef CONFIG_DECSTATION
CORE_FILES    += arch/mips/dec/dec.o
SUBDIRS       += arch/mips/dec arch/mips/dec/prom
LIBS          += arch/mips/dec/prom/rexlib.a
LOADADDR      += 0x80040000
endif

#
# Acer PICA 61, Mips Magnum 4000 and Olivetti M700.
#
ifdef CONFIG_MIPS_JAZZ
CORE_FILES    += arch/mips/jazz/jazz.o
SUBDIRS       += arch/mips/jazz arch/mips/arc
LIBS          += arch/mips/arc/arclib.a
LOADADDR      += 0x80080000
endif

ifdef CONFIG_SNI_RM200_PCI
CORE_FILES    += arch/mips/sni/sni.o
SUBDIRS       += arch/mips/sni arch/mips/arc
LIBS          += arch/mips/arc/arclib.a
LOADADDR      += 0x80080000
endif

ifdef CONFIG_SGI
LIBS          += arch/mips/sgi/kernel/sgikern.a arch/mips/arc/arclib.a
SUBDIRS       += arch/mips/sgi/kernel arch/mips/arc
#
# Set LOADADDR to >= 0x88069000 if you want to leave space for symmon,
# 0x88002000 for production kernels.  Note that the value must be
# 8kb aligned or the handling of the current variable will break.
#
LOADADDR      += 0x88002000
HOSTCC        = cc
endif

#
# Baget/MIPS
#
ifdef CONFIG_BAGET_MIPS
SUBDIRS       += arch/mips/baget arch/mips/baget/prom
LIBS          += arch/mips/baget/baget.a arch/mips/baget/prom/bagetlib.a
endif

#
# PlayStation 2
#
ifdef CONFIG_PS2
CORE_FILES    += arch/mips/ps2/kernel/ps2.o
LIBS          += arch/mips/ps2/sbios/sbioslib.a
SUBDIRS       += arch/mips/ps2/kernel arch/mips/ps2/sbios
LOADADDR      += 0x80010000
endif

#
# Choosing incompatible machines durings configuration will result in
# error messages during linking.  Select a default linkscript if
# none has been choosen above.
#
ifndef LINKSCRIPT
ifndef CONFIG_CPU_LITTLE_ENDIAN
LINKSCRIPT    = arch/mips/ld.script.big
else
LINKSCRIPT    = arch/mips/ld.script.little
endif
endif
LINKFLAGS     += -T $(word 1,$(LINKSCRIPT))

ifdef LOADADDR
LINKFLAGS     += -Ttext $(word 1,$(LOADADDR))
endif

#
# The pipe options is bad for my low-mem machine
# Uncomment this if you want this.
#
CFLAGS		+= -pipe

HEAD := arch/mips/kernel/head.o arch/mips/kernel/init_task.o

#SUBDIRS := $(SUBDIRS) $(addprefix arch/mips/, kernel mm lib tools)
SUBDIRS :=  $(addprefix arch/mips/, tools) $(SUBDIRS) \
		$(addprefix arch/mips/, kernel mm lib)
CORE_FILES := arch/mips/kernel/kernel.o arch/mips/mm/mm.o $(CORE_FILES)
LIBS := arch/mips/lib/lib.a $(LIBS)

ifdef CONFIG_BAGET_MIPS

BAGETBOOT = $(MAKE) -C arch/$(ARCH)/baget

balo:  vmlinux
	$(BAGETBOOT) balo

endif

MAKEBOOT = $(MAKE) -C arch/$(ARCH)/boot

zImage: vmlinux
	@$(MAKEBOOT) zImage

compressed: zImage

zdisk: vmlinux
	@$(MAKEBOOT) zdisk

archclean:
	@$(MAKEBOOT) clean
	$(MAKE) -C arch/$(ARCH)/kernel clean
	$(MAKE) -C arch/$(ARCH)/tools clean

archmrproper:

archdep:
	@$(MAKEBOOT) dep
