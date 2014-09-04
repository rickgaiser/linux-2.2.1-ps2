#ifndef __PS2_DEV_H
#define __PS2_DEV_H

#include <linux/ioctl.h>
#include <asm/types.h>
#include <linux/ps2/ee.h>
#include <linux/ps2/gs.h>
#include <linux/ps2/pad.h>

#define PS2_DEV_EVENT	"/dev/ps2event"
#define PS2_DEV_MEM	"/dev/ps2mem"
#define PS2_DEV_GS	"/dev/ps2gs"
#define PS2_DEV_VPU0	"/dev/ps2vpu0"
#define PS2_DEV_VPU1	"/dev/ps2vpu1"
#define PS2_DEV_IPU	"/dev/ps2ipu"
#define PS2_DEV_SPR	"/dev/ps2spr"
#define PS2_DEV_PAD0	"/dev/ps2pad00"
#define PS2_DEV_PAD1	"/dev/ps2pad10"
#define PS2_DEV_PADSTAT	"/dev/ps2padstat"

/* event number defines */

#define PS2EV_N_MAX		16
#define PS2EV_N_VBSTART		0
#define PS2EV_N_VBEND		1
#define PS2EV_N_VIF0		2
#define PS2EV_N_VIF1		3
#define PS2EV_N_VU0		4
#define PS2EV_N_VU1		5
#define PS2EV_N_IPU		6
#define PS2EV_N_SIGNAL		8
#define PS2EV_N_FINISH		9
#define PS2EV_N_HSYNC		10
#define PS2EV_N_VSYNC		11
#define PS2EV_N_EDW		12
#define PS2EV_N_ALL		-1

#define PS2EV_VBSTART		(1 << PS2EV_N_VBSTART)
#define PS2EV_VBEND		(1 << PS2EV_N_VBEND)
#define PS2EV_VIF0		(1 << PS2EV_N_VIF0)
#define PS2EV_VIF1		(1 << PS2EV_N_VIF1)
#define PS2EV_VU0		(1 << PS2EV_N_VU0)
#define PS2EV_VU1		(1 << PS2EV_N_VU1)
#define PS2EV_IPU		(1 << PS2EV_N_IPU)
#define PS2EV_SIGNAL		(1 << PS2EV_N_SIGNAL)
#define PS2EV_FINISH		(1 << PS2EV_N_FINISH)
#define PS2EV_HSYNC		(1 << PS2EV_N_HSYNC)
#define PS2EV_VSYNC		(1 << PS2EV_N_VSYNC)
#define PS2EV_EDW		(1 << PS2EV_N_EDW)
#define PS2EV_GET		-1
#define PS2EV_ALL		0xffff

#define PS2EV_SENDDONE		(1 << 0)
#define PS2EV_RECVDONE		(1 << 1)

/* misc. defines */

#define PS2_GSRESET_GIF		0
#define PS2_GSRESET_GS		1
#define PS2_GSRESET_FULL	2

/* structures for ioctl argument */

struct ps2_packet {
    void *ptr;
    unsigned int len;
};

struct ps2_packet_spr {
    void *ptr;
    unsigned short len;
    __u16 offset;
};

struct ps2_plist {
    int num;
    struct ps2_packet *packet;
};

struct ps2_plist_spr {
    int num;
    struct ps2_packet_spr *packet;
};

struct ps2_pstop {
    void *ptr;
    int qct;
};

struct ps2_image {
    void *ptr;
    int fbp;
    int fbw;
    int psm;
    int x, y;
    int w, h;
};

struct ps2_gssreg {
    __u64 val;
    int reg;
};

struct ps2_gsreg {
    __u64 val;
    int reg;
};

struct ps2_gifreg {
    __u32 val;
    int reg;
};

struct ps2_screeninfo {
    int mode;
    int res;
    int w, h;
    int fbp;
    int psm;
};

struct ps2_crtmode {
    int mode;
    int res;
};

struct ps2_display {
    int ch;
    int w, h;
    int dx, dy;
};

struct ps2_dispfb {
    int ch;
    int fbp;
    int fbw;
    int psm;
    int dbx, dby;
};

struct ps2_pmode {
    int sw;
    int mmod, amod, slbg;
    int alp;
};

struct ps2_vifreg {
    __u32 val;
    int reg;
};

struct ps2_fifo {
    __u8 data[16];
};

struct ps2_gsinfo {
    unsigned long size;
};

struct ps2_vpuinfo {
    unsigned long umemsize;
    unsigned long vumemsize;
};

struct ps2_sprinfo {
    unsigned long size;
};

/* ioctl function number defines */

#define PS2IOC_MAGIC		0xee

/* ps2event */
#define PS2IOC_ENABLEEVENT	_IO(PS2IOC_MAGIC, 0)
#define PS2IOC_GETEVENT		_IO(PS2IOC_MAGIC, 1)
#define PS2IOC_WAITEVENT	_IO(PS2IOC_MAGIC, 2)
#define PS2IOC_EVENTCOUNT	_IO(PS2IOC_MAGIC, 3)
#define PS2IOC_HSYNCACT		_IO(PS2IOC_MAGIC, 4)
#define PS2IOC_GETHSYNC		_IO(PS2IOC_MAGIC, 5)
#define PS2IOC_SETSIGNAL	_IO(PS2IOC_MAGIC, 6)

/* common DMA functions */
#define PS2IOC_SEND		_IOW(PS2IOC_MAGIC, 16, struct ps2_packet)
#define PS2IOC_SENDA		_IOW(PS2IOC_MAGIC, 17, struct ps2_packet)
#define PS2IOC_SENDL		_IOW(PS2IOC_MAGIC, 18, struct ps2_plist)
#define PS2IOC_SENDQCT		_IO(PS2IOC_MAGIC, 19)
#define PS2IOC_SENDSTOP		_IOW(PS2IOC_MAGIC, 20, struct ps2_pstop)
#define PS2IOC_SENDLIMIT	_IO(PS2IOC_MAGIC, 21)
#define PS2IOC_RECV		_IOW(PS2IOC_MAGIC, 24, struct ps2_packet)
#define PS2IOC_RECVA		_IOW(PS2IOC_MAGIC, 25, struct ps2_packet)
#define PS2IOC_RECVL		_IOW(PS2IOC_MAGIC, 26, struct ps2_plist)
#define PS2IOC_RECVQCT		_IO(PS2IOC_MAGIC, 27)
#define PS2IOC_RECVSTOP		_IOW(PS2IOC_MAGIC, 28, struct ps2_pstop)
#define PS2IOC_RECVLIMIT	_IO(PS2IOC_MAGIC, 29)

/* ps2gs */
#define PS2IOC_GSINFO		_IOR(PS2IOC_MAGIC, 32, struct ps2_gsinfo)
#define PS2IOC_GSRESET		_IO(PS2IOC_MAGIC, 33)
#define PS2IOC_LOADIMAGE	_IOW(PS2IOC_MAGIC, 34, struct ps2_image)
#define PS2IOC_STOREIMAGE	_IOW(PS2IOC_MAGIC, 35, struct ps2_image)
#define PS2IOC_SGSSREG		_IOW(PS2IOC_MAGIC, 36, struct ps2_gssreg)
#define PS2IOC_GGSSREG		_IOWR(PS2IOC_MAGIC, 37, struct ps2_gssreg)
#define PS2IOC_SGSREG		_IOW(PS2IOC_MAGIC, 38, struct ps2_gsreg)
#define PS2IOC_SGIFREG		_IOW(PS2IOC_MAGIC, 39, struct ps2_gifreg)
#define PS2IOC_GGIFREG		_IOWR(PS2IOC_MAGIC, 40, struct ps2_gifreg)
#define PS2IOC_SSCREENINFO	_IOW(PS2IOC_MAGIC, 41, struct ps2_screeninfo)
#define PS2IOC_GSCREENINFO	_IOR(PS2IOC_MAGIC, 42, struct ps2_screeninfo)
#define PS2IOC_SCRTMODE		_IOW(PS2IOC_MAGIC, 43, struct ps2_crtmode)
#define PS2IOC_GCRTMODE		_IOR(PS2IOC_MAGIC, 44, struct ps2_crtmode)
#define PS2IOC_SDISPLAY		_IOW(PS2IOC_MAGIC, 45, struct ps2_display)
#define PS2IOC_GDISPLAY		_IOWR(PS2IOC_MAGIC, 46, struct ps2_display)
#define PS2IOC_SDISPFB		_IOW(PS2IOC_MAGIC, 47, struct ps2_dispfb)
#define PS2IOC_GDISPFB		_IOWR(PS2IOC_MAGIC, 48, struct ps2_dispfb)
#define PS2IOC_SPMODE		_IOW(PS2IOC_MAGIC, 49, struct ps2_pmode)
#define PS2IOC_GPMODE		_IOR(PS2IOC_MAGIC, 50, struct ps2_pmode)
#define PS2IOC_DPMS		_IO(PS2IOC_MAGIC, 51)
#define PS2IOC_LOADIMAGEA	_IOW(PS2IOC_MAGIC, 52, struct ps2_image)

/* ps2vpu0, ps2vpu1 */
#define PS2IOC_VPUINFO		_IOR(PS2IOC_MAGIC, 64, struct ps2_vpuinfo)
#define PS2IOC_SVIFREG		_IOW(PS2IOC_MAGIC, 65, struct ps2_vifreg)
#define PS2IOC_GVIFREG		_IOWR(PS2IOC_MAGIC, 66, struct ps2_vifreg)

/* ps2ipu */
#define PS2IOC_SIPUCMD		_IOW(PS2IOC_MAGIC, 81, __u32)
#define PS2IOC_GIPUCMD		_IOR(PS2IOC_MAGIC, 82, __u32)
#define PS2IOC_SIPUCTRL		_IOW(PS2IOC_MAGIC, 83, __u32)
#define PS2IOC_GIPUCTRL		_IOR(PS2IOC_MAGIC, 84, __u32)
#define PS2IOC_GIPUTOP		_IOR(PS2IOC_MAGIC, 85, __u32)
#define PS2IOC_GIPUBP		_IOR(PS2IOC_MAGIC, 86, __u32)
#define PS2IOC_SIPUFIFO		_IOW(PS2IOC_MAGIC, 87, struct ps2_fifo)
#define PS2IOC_GIPUFIFO		_IOR(PS2IOC_MAGIC, 88, struct ps2_fifo)

/* ps2spr */
#define PS2IOC_SPRINFO		_IOR(PS2IOC_MAGIC, 96, struct ps2_sprinfo)

#endif /* __PS2_DEV_H */
