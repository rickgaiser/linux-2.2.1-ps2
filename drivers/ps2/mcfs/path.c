/*
 *
 *        Copyright (C) 2000  Sony Computer Entertainment Inc.
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License Version 2. See the file "COPYING" in the main
 * directory of this archive for more details.
 *
 * $Id: path.c,v 1.7 2000/09/26 04:21:11 takemura Exp $
 */
#include <linux/kernel.h>
#include <linux/stat.h>
#include <linux/list.h>
#include <linux/string.h>	/* for memmove */
#include <linux/fs.h>

#include "mcfs.h"
#include "mcfs_debug.h"

#define ARRAYSIZEOF(a)	(sizeof(a)/sizeof(*(a)))

static struct ps2mcfs_pathent {
	char pathname[PS2MC_PATH_MAX + 1]; /* null terminated full path name */
	struct ps2mcfs_dirent *dirent;
	struct list_head link;
	int inuse;
} items[PS2MCFS_NAME_CACHESIZE];

static struct list_head lru;

/*
 * Insert a new entry at tail of the specified list
 */
static __inline__ void list_addtail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

int
ps2mcfs_init_pathcache()
{
	int i;

	INIT_LIST_HEAD(&lru);
	for (i = 0; i < ARRAYSIZEOF(items); i++) {
		list_add(&items[i].link, &lru);
	}
	return (0);
}

int
ps2mcfs_exit_pathcache()
{
	return (0);
}

#ifdef PS2MCFS_DEBUG
static void dump(void)
{
	struct list_head *p;

	for (p = lru.prev; p != &lru; p = p->prev) {
		struct ps2mcfs_pathent *ent;
		ent = list_entry(p, struct ps2mcfs_pathent, link);
		printk(" %3d: %d %s\n", ent - items, ent->inuse, ent->pathname);
	}
}
#endif

const char *
ps2mcfs_get_path(struct ps2mcfs_dirent *dirent)
{
	struct ps2mcfs_pathent *ent;
	struct ps2mcfs_dirent *p;
	int i;

	ent = dirent->path;
	if (ent != NULL) {
		list_del(&ent->link);
		list_add(&ent->link, &lru);
		ent->inuse++;
		DPRINT(DBG_PATHCACHE, "get_path: %s(count=%d)\n",
		       ent->pathname, ent->inuse);
		return (ent->pathname);
	}

	ent = list_entry(lru.prev, struct ps2mcfs_pathent, link);
	if (0 < ent->inuse) {
		printk(KERN_CRIT "ps2mcfs_get_path(): resource not available\n");
#ifdef PS2MCFS_DEBUG
		dump();
#else
		return (NULL); /* This will make oops */
#endif
	}
	ent->inuse = 1;

	if (ent->dirent != NULL)
		ent->dirent->path = NULL;
	list_del(&ent->link);
	list_add(&ent->link, &lru);
	ent->dirent = dirent;
	dirent->path = ent;

	i = ARRAYSIZEOF(ent->pathname) - 1;
	ent->pathname[i] = '\0'; /* terminator */
	for (p = dirent; p != NULL; p = p->parent) {
		i -= p->namelen;
		if (i < 1) {
			printk("ps2mcfs: path name is too long\n");
			printk("...%s\n", &ent->pathname[i]);
			ent->pathname[0] = '\0';
			/* XXX, caller shouldn't call put_path() */
			ent->inuse = 0;
			break;
		}
		memcpy(&ent->pathname[i], p->name, p->namelen);
		if (p->namelen != 0)
			ent->pathname[--i] = '/';
	}
	if (ent->pathname[i] != '/')
		ent->pathname[--i] = '/';
	memmove(&ent->pathname[0], &ent->pathname[i],
		ARRAYSIZEOF(ent->pathname) - i);

	DPRINT(DBG_PATHCACHE, "get_path: %s\n", ent->pathname);

	return (ent->pathname);
}

void
ps2mcfs_put_path(struct ps2mcfs_dirent *dirent, const char *path)
{
	if (dirent->path == NULL)
		return;
#ifdef PS2MCFS_DEBUG
	if (dirent->path->pathname != path)
		DPRINT(DBG_PATHCACHE, "ps2mcfs_put_path: something wrong...\n");
#endif
	dirent->path->inuse--;
	DPRINT(DBG_PATHCACHE, "put_path: %s(count=%d)\n",
	       dirent->path->pathname, dirent->path->inuse);
}

void
ps2mcfs_free_path(struct ps2mcfs_dirent *dirent)
{
	struct ps2mcfs_pathent *ent;

	if (dirent->path == NULL)
		return;
	ent = dirent->path;
#ifdef PS2MCFS_DEBUG
	DPRINT(DBG_PATHCACHE, "ps2mcfs_free_path: %s(count=%d)\n",
	       ent->pathname, ent->inuse);
	if (ent->inuse)
		printk(KERN_CRIT "ps2mcfs: path name '%s' refcount != 0\n",
		       ent->pathname);
#endif
	ent->dirent = NULL;
	dirent->path = NULL;
	list_del(&ent->link);
	list_addtail(&ent->link, &lru);
}
