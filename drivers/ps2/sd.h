/*
 *  PlayStation 2 Sound driver
 *
 *        Copyright (C) 2000, 2001  Sony Computer Entertainment Inc.
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License Version 2. See the file "COPYING" in the main
 * directory of this archive for more details.
 *
 * $Id: sd.h,v 1.20.2.2 2001/09/19 10:08:23 takemura Exp $
 */

#ifndef PS2SD_H
#define PS2SD_H

#include <asm/ps2/debuglog.h>

/*
 * macro defines
 */
#define PS2SD_DEBUG
#ifdef PS2SD_DEBUG

#define DBG_VERBOSE	(1<< 0)
#define DBG_INFO	(1<< 1)
#define DBG_INTR	(1<< 2)
#define DBG_DIAG	(1<< 3)
#define DBG_WRITE	(1<< 4)
#define DBG_MIXER	(1<< 5)
#define DBG_IOCTL	(1<< 6)
#define DBG_DMASTAT	(1<< 7)
#define DBG_TRACE	(1<< 8)
#define DBG_RPC		(1<< 9)
#define DBG_RPCSVC	(1<<10)

#define DBG_THROUGH	(1<<31)

#define DBG_LOG_LEVEL	KERN_CRIT

#define DEBUGLOG(fmt, args...)		debuglog(NULL, fmt, ## args)

#define DPRINT(mask, fmt, args...) \
	do { \
		if ((ps2sd_debug & (mask)) == (mask)) { \
			if (ps2sd_debug & DBG_THROUGH) \
				printk(DBG_LOG_LEVEL "ps2sd: " fmt, ## args); \
			else \
				DEBUGLOG(fmt, ## args); \
		} \
	} while (0)
#define DPRINTK(mask, fmt, args...) \
	do { \
		if ((ps2sd_debug & (mask)) == (mask)) { \
			if (ps2sd_debug & DBG_THROUGH) \
				printk(fmt, ## args); \
			else \
				DEBUGLOG(fmt, ## args); \
		} \
	} while (0)
#else
#define DPRINT(mask, fmt, args...) do {} while (0)
#endif

#define TRACE(fmt, args...) DPRINT(DBG_TRACE, fmt, ## args)
#define TRACE2(fmt, args...) DPRINT(DBG_TRACE|DBG_VERBOSE, fmt, ## args)

#define ARRAYSIZEOF(a)	(sizeof(a)/sizeof(*(a)))
#define ALIGN(a, n)	((__typeof__(a))(((unsigned long)(a) + (n) - 1) / (n) * (n)))

#define PS2SD_DEVC(filp)	((struct ps2sd_unit_context *)(filp)->private_data)

/*
 * types
 */
struct ps2sd_module_context {
	unsigned int init;
	ps2sif_lock_t *lock;	/* the lock which is need to RPC and
				   transfer DMA status */
	struct ps2sd_unit_context *lock_owner;
	void *iopzero;
};

struct ps2sd_mixer_context;
struct ps2sd_mixer_channel {
	int vol, volr, voll;
#ifdef SPU2_REG_DIRECT_ACCESS
	volatile short *regr, *regl;
#else
	int regr, regl;
#endif
	long scale;
	char *name;
	struct ps2sd_mixer_context *mixer;
};

#ifdef SOUND_MIXER_NRDEVICES
struct ps2sd_mixer_context {
	/* Mixer stuff */
	int mixer;
	int modified;
	long devmask;
	struct ps2sd_mixer_channel *channels[SOUND_MIXER_NRDEVICES];
};
#endif

struct ps2sd_iopbuf {
	int size;
	long iopaddr;
	unsigned int dmaid;
};

struct ps2sd_sample {
	short l, r;
};

struct ps2sd_unit_context {
	unsigned int init;
	int dsp;
	int flags;
	int init_flags;
#define PS2SD_UNIT_OPENED	(1<<0)
#define PS2SD_UNIT_INT512	(1<<1)
#define PS2SD_UNIT_MEMIN32	(1<<2)
#define PS2SD_UNIT_EXCLUSIVE	(1<<3)
	int core;
	int dmach;
	struct wait_queue *waitq;
	spinlock_t spinlock;
	int dmastat;
	int total_output_bytes;
	ps2sif_lock_queue_t lockq;
        struct timer_list timer;
	int intr_count;
	int timeout;
	char *debug_check_point;

	/* Mixer stuff */
	struct ps2sd_mixer_channel mixer_main;
	struct ps2sd_mixer_channel mixer_pcm;
	struct ps2sd_mixer_channel mixer_extrn;

	/* IOP->SPU DMA stuff */
	int iopbufsize;
	long iopbuf;
	struct ps2sd_iopbuf iopbufs[2];
	struct ps2sd_iopbuf *fg, *bg;

	/* main memory->IOP DMA stuff */
	int dmabufsize;
	int dmabufcount;
	int dmabufhead;
	int dmabuftail;
	int dmabufunderflow;
	unsigned char *dmabuf;
	int phaseerr;

	/* format */
	int format;
	int speed;
	int stereo;
	int requested_fragsize;
	int requested_maxfrags;

	/* format conversion stuff */
	int samplesize;
	void (*fetch)(struct ps2sd_sample *, unsigned char *);
	int noconversion;

	int cnvbufcount;
	int cnvbufhead;
	int cnvbuftail;
	unsigned char *cnvbuf;
	long cnvd;
	long cnvsrcrate;
	long cnvdstrate;
	int intbufcount;
	unsigned char *intbuf;
};

/*
 * function prototypes
 */
struct ps2sd_unit_context *ps2sd_lookup_by_dsp(int dsp);
struct ps2sd_unit_context *ps2sd_lookup_by_dmach(int dmach);
struct ps2sd_mixer_context *ps2sd_lookup_mixer(int mixer);

int ps2sdmixer_setvol(struct ps2sd_mixer_channel *ch, int volr, int voll);
int ps2sdmixer_do_ioctl(struct ps2sd_mixer_context *, unsigned int,
			unsigned long);

/*
 * variables
 */
extern unsigned long ps2sd_debug;
extern struct ps2sd_module_context ps2sd_mc;
extern struct ps2sd_unit_context ps2sd_units[];
extern int ps2sd_nunits;
extern struct file_operations ps2sd_mixer_fops;

#endif /* PS2SD_H */
