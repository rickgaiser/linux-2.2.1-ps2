/*
 *
 *        Copyright (C) 2000  Sony Computer Entertainment Inc.
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License Version 2. See the file "COPYING" in the main
 * directory of this archive for more details.
 *
 * $Id: mcfs.h,v 1.13 2000/10/12 12:23:57 takemura Exp $
 */
#ifndef _PS2MCFS_H_
#define _PS2MCFS_H_

#include <linux/ps2/mcio.h>

#define PS2MCFS_FD_EXPIRE_TIME	(HZ/2)
#define PS2MCFS_CHECK_INTERVAL	(PS2MCFS_FD_EXPIRE_TIME/3)
#define PS2MCFS_SUPER_MAGIC	0xaaaa
#define PS2MCFS_NAME_CACHESIZE	30

#define PS2MCFS_DIRENT_INVALID	(1<<0)
#define PS2MCFS_DIRENT_BMAPPED	(1<<1)

#define PS2MCFS_DIRENT_BITS	8
#define PS2MCFS_SECTOR_BITS	24
#define PS2MCFS_DIRENT_SHIFT	PS2MCFS_SECTOR_BITS
#define PS2MCFS_SECTOR_SHIFT	0

#define PS2MCFS_MAX_DIRENTS	(1 << PS2MCFS_DIRENT_BITS)
#define PS2MCFS_DIRENT_MASK	(~(1<<PS2MCFS_DIRENT_BITS))
#define PS2MCFS_SECTOR_MASK	(~(1<<PS2MCFS_SECTOR_BITS))

struct ps2mcfs_options {
	uid_t uid;
	gid_t gid;
	unsigned short umask;
};

struct ps2mcfs_root {
	int portslot;
	int refcount;
	struct ps2mcfs_dirent *dirent;
	struct ps2mcfs_options opts;
	int block_shift;
	kdev_t dev;
};

struct ps2mcfs_dirent {
	int no;
	unsigned long ino;
	struct ps2mcfs_root *root;
	char name[PS2MC_NAME_MAX];
	struct ps2mcfs_pathent *path;
	struct ps2mcfs_filedesc *fd;
	unsigned short namelen;
	unsigned long size;
	struct inode *inode;
	struct ps2mcfs_dirent *parent;
	struct list_head next, sub, hashlink;
	int refcount;
	int flags;
};
extern struct inode_operations ps2mcfs_dir_inode_operations;
extern struct inode_operations ps2mcfs_file_inode_operations;
extern struct inode_operations ps2mcfs_null_inode_operations;
extern char *ps2mcfs_basedir;

int ps2mcfs_init_dirent(void);
int ps2mcfs_init_root(void);
int ps2mcfs_exit_root(void);
int ps2mcfs_init_pathcache(void);
int ps2mcfs_exit_pathcache(void);
int ps2mcfs_init_fdcache(void);
int ps2mcfs_exit_fdcache(void);
const char *ps2mcfs_get_path(struct ps2mcfs_dirent *);
void ps2mcfs_put_path(struct ps2mcfs_dirent *, const char *path);
void ps2mcfs_free_path(struct ps2mcfs_dirent *);
int ps2mcfs_get_fd(struct ps2mcfs_dirent *, int);
void ps2mcfs_put_fd(struct ps2mcfs_dirent *, int);
void ps2mcfs_free_fd(struct ps2mcfs_dirent *);
void ps2mcfs_check_fd(void);
int ps2mcfs_init_filebuf(void);
int ps2mcfs_exit_filebuf(void);
int ps2mcfs_blkrw(int, int, void*, int);

int ps2mcfs_get_root(kdev_t, struct ps2mcfs_root **);
int ps2mcfs_put_root(int);
unsigned long ps2mcfs_pseudo_ino(void);
int ps2mcfs_is_pseudo_ino(unsigned long);
int ps2mcfs_countdir(struct ps2mcfs_dirent *);
int ps2mcfs_update_inode(struct inode * inode);
int ps2mcfs_setup_fake_root(struct ps2mcfs_root*);
void ps2mcfs_invalidate_dirents(struct ps2mcfs_root *);

char* ps2mcfs_terminate_name(const char *, int);
struct ps2mcfs_dirent* ps2mcfs_alloc_dirent(struct ps2mcfs_dirent *, const char *, int);
struct ps2mcfs_dirent* ps2mcfs_find_dirent(struct ps2mcfs_dirent *, const char *, int);
struct ps2mcfs_dirent* ps2mcfs_find_dirent_ino(unsigned long);
struct ps2mcfs_dirent* ps2mcfs_find_dirent_no(int no);
void ps2mcfs_ref_dirent(struct ps2mcfs_dirent *);
void ps2mcfs_unref_dirent(struct ps2mcfs_dirent *);
void ps2mcfs_free_dirent(struct ps2mcfs_dirent *);

struct super_block *ps2mcfs_read_super(struct super_block *, void *, int);
int ps2mcfs_create(struct ps2mcfs_dirent *);

#endif /* _PS2MCFS_H_ */
