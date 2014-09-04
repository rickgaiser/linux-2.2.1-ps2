/*
 *  PlayStation 2 Mixer driver
 *
 *        Copyright (C) 2000, 2001  Sony Computer Entertainment Inc.
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License Version 2. See the file "COPYING" in the main
 * directory of this archive for more details.
 *
 * $Id: sdmixer.c,v 1.8.6.3 2001/09/19 10:08:23 takemura Exp $
 */

#define __NO_VERSION__
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/malloc.h>
#include <linux/interrupt.h>
#include <linux/soundcard.h>
#include <asm/io.h>
#include <asm/addrspace.h>
#include <asm/uaccess.h>
#include <asm/spinlock.h>
#include <asm/ps2/sifdefs.h>
#include <asm/ps2/siflock.h>
#include "../sound/soundmodule.h"
#include "../sound/sound_config.h"
#include "sd.h"
#include "sdmacro.h"
#include "sdcall.h"

/*
 * macro defines
 */

/*
 * data types
 */

/*
 * function prototypes
 */
static loff_t ps2sdmixer_llseek(struct file *, loff_t, int);
static int ps2sdmixer_open(struct inode *, struct file *);
static int ps2sdmixer_release(struct inode *, struct file *);
static int ps2sdmixer_ioctl(struct inode *, struct file *, unsigned int, unsigned long);

/*
 * variables and data
 */
struct file_operations ps2sd_mixer_fops = {
	ps2sdmixer_llseek,
	NULL,			/* read */
	NULL,			/* write */
	NULL,			/* readdir */
	NULL,			/* poll */
	ps2sdmixer_ioctl,
	NULL,			/* mmap */
	&ps2sdmixer_open,
	NULL,			/* flush */
	ps2sdmixer_release,
	NULL,			/* fsync */
	NULL,			/* fasync */
	NULL,			/* check_media_change */
	NULL,			/* revalidate */
	NULL,			/* lock */
};

/*
 * function bodies
 */
static loff_t
ps2sdmixer_llseek(struct file *filp, loff_t offset, int origin)
{
	return -ESPIPE;
}

int
ps2sdmixer_do_ioctl(struct ps2sd_mixer_context *mixer,
		    unsigned int cmd, unsigned long arg)
{
	int i, res, val;

	/*
	 * get device driver info
	 */
        if (cmd == SOUND_MIXER_INFO) {
		mixer_info info;
		strncpy(info.id, "PS2SPU", sizeof(info.id));
		strncpy(info.name, "PS2 Sound Processing Unit",
			sizeof(info.name));
		info.modify_counter = mixer->modified;
		if (copy_to_user((void *)arg, &info, sizeof(info)))
			return -EFAULT;
		return 0;
	}
	if (cmd == SOUND_OLD_MIXER_INFO) {
		_old_mixer_info info;
		strncpy(info.id, "PS2SPU", sizeof(info.id));
		strncpy(info.name, "PS2 Sound Processing Unit",
			sizeof(info.name));
		if (copy_to_user((void *)arg, &info, sizeof(info)))
			return -EFAULT;
		return 0;
	}
	if (cmd == OSS_GETVERSION)
		return put_user(SOUND_VERSION, (int *)arg);

	if (_IOC_TYPE(cmd) != 'M' || _IOC_SIZE(cmd) != sizeof(int))
                return -EINVAL;

	/*
	 * get channel info
	 */
        if (_IOC_DIR(cmd) == _IOC_READ) {
                switch (_IOC_NR(cmd)) {
                case SOUND_MIXER_RECSRC:
			/* SPU2 have no recording source */
			return put_user(0, (int *)arg);
			
                case SOUND_MIXER_DEVMASK:
			/* supported devices */
			return put_user(mixer->devmask, (int *)arg);

                case SOUND_MIXER_RECMASK:
			/* SPU2 have no recording input */
			return put_user(0, (int *)arg);
			
                case SOUND_MIXER_STEREODEVS:
			/* Mixer channels supporting stereo */
			return put_user(mixer->devmask, (int *)arg);
			
                case SOUND_MIXER_CAPS:
			return put_user(0, (int *)arg);

		default:
			/* get current volume */
			i = _IOC_NR(cmd);
                        if (SOUND_MIXER_NRDEVICES <= i ||
			    mixer->channels[i] == NULL)
                                return -EINVAL;
			return put_user(mixer->channels[i]->vol, (int *)arg);
		}
	}

        if (_IOC_DIR(cmd) != (_IOC_READ|_IOC_WRITE)) 
		return -EINVAL;

	/*
	 * set channel info
	 */
	switch (_IOC_NR(cmd)) {
	case SOUND_MIXER_RECSRC:
		/* SPU2 have no recording source */
		return 0;

	default:
		i = _IOC_NR(cmd);
		if (SOUND_MIXER_NRDEVICES <= i || 
		    mixer->channels[i] == NULL)
			return -EINVAL;
		get_user_ret(val, (int *)arg, -EFAULT);

		res = ps2sdmixer_setvol(mixer->channels[i],
					(val >> 8) & 0xff, val & 0xff);
		if (res < 0) return res;

                return put_user(mixer->channels[i]->vol, (int *)arg);
	}
}

static int
ps2sdmixer_open(struct inode *inode, struct file *filp)
{
	int minor = MINOR(inode->i_rdev);
	struct ps2sd_mixer_context *mixer;

	DPRINT(DBG_INFO, "open: inode->i_rdev=0x%04x\n", inode->i_rdev);
	mixer = ps2sd_lookup_mixer(minor);
	if (mixer == NULL) return -ENODEV;

	filp->private_data = mixer;

	MOD_INC_USE_COUNT;
	return 0;
}

static int
ps2sdmixer_release(struct inode *inode, struct file *filp)
{

	MOD_DEC_USE_COUNT;
	return 0;
}

static int
ps2sdmixer_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct ps2sd_mixer_context *mixer;

	mixer = (struct ps2sd_mixer_context *)filp->private_data;
	return ps2sdmixer_do_ioctl(mixer, cmd, arg);
}

int
ps2sdmixer_setvol(struct ps2sd_mixer_channel *ch, int volr, int voll)
{
	int res;

	if (volr < 0) volr = 0;
	if (100 < volr) volr = 100;
	if (voll < 0) voll = 0;
	if (100 < voll) voll = 100;

	if (ch->mixer != NULL)
		ch->mixer->modified++;
	ch->volr = volr;
	ch->voll = voll;
	ch->vol = (volr << 8) | voll;

	volr = volr * ch->scale / 100;
	voll = voll * ch->scale / 100;

	DPRINT(DBG_MIXER, "%14s R=%04x/L=%04x\n", ch->name, volr, voll);

	if ((res = ps2sif_lock(ps2sd_mc.lock, "mixer volume")) < 0) {
		return res;
	}

	if (ch->regr != -1) {
	  res = ps2sdcall_set_reg(ch->regr, volr);
	  if (res < 0) {
		DPRINT(DBG_DIAG, "%s R=%04x/L=%04x: failed, res=%d\n",
		       ch->name, volr, voll, res);
		goto unlock_and_return;
	  }
	}
	if (ch->regl != -1) {
	  res = ps2sdcall_set_reg(ch->regl, voll);
	  if (res < 0) {
		DPRINT(DBG_DIAG, "%s R=%04x/L=%04x: failed, res=%d\n",
		       ch->name, volr, voll, res);
		goto unlock_and_return;
	  }
	}

 unlock_and_return:
	ps2sif_unlock(ps2sd_mc.lock);

	return res;
}
