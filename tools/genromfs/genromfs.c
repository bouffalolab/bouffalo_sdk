// x86_64-w64-mingw32-gcc --static genromfs.c -o genromfs.exe
// gcc --static genromfs.c -o genromfs

#include <stdint.h>
#include <stdio.h>  /* Userland pieces of the ANSI C standard I/O package  */
#include <stdlib.h> /* Userland prototypes of the ANSI C std lib functions */
#include <string.h> /* Userland prototypes of the string handling funcs    */
#include <time.h>

#include <unistd.h> /* Userland prototypes of the Unix std system calls    */
#include <fcntl.h>  /* Flag value for file handling functions              */
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#define PATH_SEPARATOR '/'
#define lstat stat

struct romfh {
	int32_t nextfh;
	int32_t spec;
	int32_t size;
	int32_t checksum;
};

#define ROMFS_MAXFN 128
#define ROMFH_HRD 0
#define ROMFH_DIR 1
#define ROMFH_REG 2
#define ROMFH_LNK 3
#define ROMFH_BLK 4
#define ROMFH_CHR 5
#define ROMFH_SCK 6
#define ROMFH_FIF 7
#define ROMFH_EXEC 8

struct filenode;

struct filehdr {
	/* leave h, t, tp at front, this is a linked list header */
	struct filenode *head;
	struct filenode *tail;
	struct filenode *tailpred;
	/* end fragile header */
	struct filenode *owner;
};

struct filenode {
	/* leave n, p at front, this is a linked list item */
	struct filenode *next;
	struct filenode *prev;
	/* end fragile header */
	struct filenode *parent;
	struct filehdr dirlist;
	struct filenode *orig_link;
	char *name;
	char *realname;
	dev_t ondev;
	dev_t devnode;
	ino_t onino;
	mode_t modes;
	unsigned int offset;
	unsigned int size;
	unsigned int pad;
};

static uint32_t __ntohl(uint32_t netlong) {
    return ((netlong & 0xFF000000) >> 24) |
           ((netlong & 0x00FF0000) >> 8)  |
           ((netlong & 0x0000FF00) << 8)  |
           ((netlong & 0x000000FF) << 24);
}

static uint32_t __htonl(uint32_t hostlong) {
    return ((hostlong & 0xFF000000) >> 24) |
           ((hostlong & 0x00FF0000) >> 8)  |
           ((hostlong & 0x0000FF00) << 8)  |
           ((hostlong & 0x000000FF) << 24);
}

void initlist(struct filehdr *fh, struct filenode *owner)
{
	fh->head = (struct filenode *)&fh->tail;
	fh->tail = NULL;
	fh->tailpred = (struct filenode *)&fh->head;
	fh->owner = owner;
}

int listisempty(struct filehdr *fh)
{
	return fh->head == (struct filenode *)&fh->tail;
}

void append(struct filehdr *fh, struct filenode *n)
{
	struct filenode *tail = (struct filenode *)&fh->tail;

	n->next = tail; n->prev = tail->prev;
	tail->prev = n; n->prev->next =n;
	n->parent = fh->owner;
}

void shownode(int level, struct filenode *node, FILE *f)
{
	struct filenode *p;
	fprintf(f, "%-4d %-20s [0x%-8x, 0x%-8x] %07o, sz %5u, at 0x%-6x",
		level, node->name,
		(int)node->ondev, (int)node->onino, node->modes, node->size,
		node->offset);

	if (node->orig_link)
		fprintf(f, " [link to 0x%-6x]", node->orig_link->offset);
	fprintf(f, "\n");

	p = node->dirlist.head;
	while (p->next) {
		shownode(level+1, p, f);
		p = p->next;
	}
}

/* Dumping functions */

static char bigbuf[4096];
static char fixbuf[512];
static int atoffs = 0;
static int align = 16;
int realbase;

int findalign(struct filenode *node)
{
	int i;

	if (S_ISREG(node->modes)) i = align;
	else i = 16;

	return i;
}

int romfs_checksum(void *data, int size)
{
        int32_t sum, *ptr;

        sum = 0; ptr = data;
        size>>=2;
        while (size>0) {
                sum += __ntohl(*ptr++);
                size--;
        }
        return sum;
}

void fixsum(struct romfh *ri, int size)
{
	ri->checksum = 0;
	ri->checksum = __htonl(-romfs_checksum(ri, size));
}

void dumpdata(void *addr, int len, FILE *f)
{
	int tocopy;
	struct romfh *ri;

	if (!len)
		return;
	if (atoffs >= 512) {
		fwrite(addr, len, 1, f);
		atoffs+=len;
		return;
	}

	tocopy = len < 512-atoffs ? len : 512-atoffs;
	memcpy(fixbuf+atoffs, addr, tocopy);
	atoffs+=tocopy;
	addr=(char*)addr+tocopy;
	len-=tocopy;

	if (atoffs==512) {
		ri = (struct romfh *)&fixbuf;
		fixsum(ri, atoffs<__ntohl(ri->size)?atoffs:__ntohl(ri->size));
		fwrite(fixbuf, atoffs, 1, f);
	}
	if (len) {
		fwrite(addr, len, 1, f);
		atoffs+=len;
	}
}

void dumpzero(int len, FILE *f)
{
	memset(bigbuf, 0, len);
	dumpdata(bigbuf, len, f);
}

void dumpdataa(void *addr, int len, FILE *f)
{
	dumpdata(addr, len, f);
	if ((len & 15) != 0)
		dumpzero(16-(len&15), f);
}

void dumpstring(char *str, FILE *f)
{
	dumpdataa(str, strlen(str)+1, f);
}

void dumpri(struct romfh *ri, struct filenode *n, FILE *f)
{
	int len;

	len = strlen(n->name)+1;
	memcpy(bigbuf, ri, sizeof(*ri));
	memcpy(bigbuf+16, n->name, len);
	if (len&15) {
		memset(bigbuf+16+len, 0, 16-(len&15));
		len += 16-(len&15);
	}
	len+=16;
	ri=(struct romfh *)bigbuf;
	if (n->offset)
		fixsum(ri, len);
	dumpdata(bigbuf, len, f);
/*
	fprintf(stderr, "RI: [at %06x] %08lx, %08lx, %08lx, %08lx [%s]\n",
		n->offset,
		__ntohl(ri->nextfh), __ntohl(ri->spec),
		__ntohl(ri->size), __ntohl(ri->checksum),
		n->name);
*/
}

void dumpnode(struct filenode *node, FILE *f)
{
	struct romfh ri;
	struct filenode *p;

	ri.nextfh = 0;
	ri.spec = 0;
	ri.size = __htonl(node->size);
	ri.checksum = __htonl(0x55555555);
	if (node->pad)
		dumpzero(node->pad, f);
	if (node->next && node->next->next)
		ri.nextfh = __htonl(node->next->offset);
	if ((node->modes & 0111) &&
	    (S_ISDIR(node->modes) || S_ISREG(node->modes)))
		ri.nextfh |= __htonl(ROMFH_EXEC);

	if (node->orig_link) {
		ri.nextfh |= __htonl(ROMFH_HRD);
		/* Don't allow hardlinks to convey attributes */
		ri.nextfh &= ~__htonl(ROMFH_EXEC);
		ri.spec = __htonl(node->orig_link->offset);
		dumpri(&ri, node, f);
	} else if (S_ISDIR(node->modes)) {
		ri.nextfh |= __htonl(ROMFH_DIR);
		if (listisempty(&node->dirlist)) {
			ri.spec = __htonl(node->offset);
		} else {
			ri.spec = __htonl(node->dirlist.head->offset);
		}
		dumpri(&ri, node, f);
	} else if (S_ISREG(node->modes)) {
		int offset, len, max, avail;
		FILE *fp;
		ri.nextfh |= __htonl(ROMFH_REG);
		dumpri(&ri, node, f);
		offset = 0;
		max = node->size;
		/* XXX warn about size mismatch */
		fp = fopen(node->realname, "rb");
		if (fp) {
			while(offset < max) {
				avail = max-offset < sizeof(bigbuf) ? max-offset : sizeof(bigbuf);
				len = fread(bigbuf, 1, avail, fp);
				if (len <= 0)
					break;
				dumpdata(bigbuf, len, f);
				offset+=len;
			}
			fclose(fp);
		}
		max = (max+15)&~15;
		while (offset < max) {
			avail = max-offset < sizeof(bigbuf) ? max-offset : sizeof(bigbuf);
			memset(bigbuf, 0, avail);
			dumpdata(bigbuf, avail, f);
			offset+=avail;
		}
	}

	p = node->dirlist.head;
	while (p->next) {
		dumpnode(p, f);
		p = p->next;
	}
}

void dumpall(struct filenode *node, int lastoff, FILE *f)
{
	struct romfh ri;
	struct filenode *p;

	ri.nextfh = __htonl(0x2d726f6d);
	ri.spec = __htonl(0x3166732d);
	ri.size = __htonl(lastoff);
	ri.checksum = __htonl(0x55555555);
	dumpri(&ri, node, f);
	p = node->dirlist.head;
	while (p->next) {
		dumpnode(p, f);
		p = p->next;
	}
	/* Align the whole bunch to ROMBSIZE boundary */
	if (lastoff&1023)
		dumpzero(1024-(lastoff&1023), f);
}

/* Node manipulating functions */

void freenode(struct filenode *n)
{
	/* Rare, not yet */
}

void setnode(struct filenode *n, dev_t dev, ino_t ino, mode_t um)
{
	n->ondev = dev;
	n->onino = ino;
	n->modes = um;
}

struct filenode *newnode(const char *base, const char *name, int curroffset)
{
	struct filenode *node;
	int len;
	char *str;

	node = malloc(sizeof (*node));
	if (!node) {
		fprintf(stderr,"out of memory\n");
		exit(1);
	}

	len = strlen(name);
	str = malloc(len+1);
	if (!str) {
		fprintf(stderr,"out of memory\n");
		exit(1);
	}
	strcpy(str, name);
	node->name = str;

	if (!curroffset) {
		len = 1;
		name = ".";
	}
	if (strlen(base))
		len++;
	str = malloc(strlen(base)+len+1);
	if (!str) {
		fprintf(stderr,"out of memory\n");
		exit(1);
	}
	if (strlen(base)) {
		sprintf(str, "%s/%s", base, name);
	} else {
		strcpy(str, name);
	}

	node->realname = str;
	node->next = node->prev = NULL;
	node->parent = NULL;
	initlist(&node->dirlist, node);

	node->ondev = -1;
	node->onino = -1;
	node->modes = -1;
	node->size = 0;
	node->devnode = 0;
	node->orig_link = NULL;
	node->offset = curroffset;
	node->pad = 0;

	return node;
}

struct filenode *findnode(struct filenode *node, dev_t dev, ino_t ino)
{
	struct filenode *found, *p;

	/* scan the whole tree */
	if (node->ondev == dev && node->onino == ino)
		return node;
	p = node->dirlist.head;
	while (p->next) {
		found = findnode(p, dev, ino);
		if (found)
			return found;
		p = p->next;
	}
	return NULL;
}

#define ALIGNUP16(x) (((x)+15)&~15)

int spaceneeded(struct filenode *node)
{
	return 16 + ALIGNUP16(strlen(node->name)+1) + ALIGNUP16(node->size);
}

int alignnode(struct filenode *node, int curroffset, int extraspace)
{
	int align = findalign(node), d;

	d = ((curroffset + extraspace) & (align - 1));
	if (d) {
		align -= d;
		curroffset += align;
		node->offset += align;
		node->pad = align;
	}
	return curroffset;
}

int processdir(int level, const char *base, const char *dirname, struct stat *sb,
	struct filenode *dir, struct filenode *root, int curroffset)
{
	DIR *dirfd;
	struct dirent *dp;
	struct filenode *n, *link;

	if (level <= 1) {
		/* Ok, to make sure . and .. are handled correctly
		 * we add them first.  Note also that we alloc them
		 * first to get to know the real name
		 */
		link = newnode(base, ".", curroffset);
		if (!lstat(link->realname, sb)) {
			setnode(link, sb->st_dev, sb->st_ino, sb->st_mode);
			append(&dir->dirlist, link);

			/* special case for root node - '..'s in subdirs should link to
			 *   '.' of root node, not root node itself.
			 */
			dir->dirlist.owner = link;

			curroffset = alignnode(link, curroffset, 0) + spaceneeded(link);
			n = newnode(base, "..", curroffset);

			if (!lstat(n->realname, sb)) {
				setnode(n, sb->st_dev, sb->st_ino, sb->st_mode);
				append(&dir->dirlist, n);
				n->orig_link = link;
				curroffset = alignnode(n, curroffset, 0) + spaceneeded(n);
			}
		}
	}

	dirfd = opendir(dir->realname);
	while((dp = readdir(dirfd))) {
		/* don't process main . and .. twice */
		if (level <= 1 &&
		    (strcmp(dp->d_name, ".") == 0
		     || strcmp(dp->d_name, "..") == 0))
			continue;
		n = newnode(base, dp->d_name, curroffset);

		if (lstat(n->realname, sb)) {
			fprintf(stderr, "ignoring '%s' (lstat failed)\n", n->realname);
			freenode(n); continue;
		}

		setnode(n, sb->st_dev, sb->st_ino, sb->st_mode);

		/* not Look up old links */
		if ( strcmp(n->name, ".") == 0 ) {
			append(&dir->dirlist, n);
			link = n->parent;
		} else if (strcmp(n->name, "..") == 0) {
			append(&dir->dirlist, n);
			link = n->parent->parent;
		} else {
			//link = findnode(root, n->ondev, n->onino);
			link = NULL; /* don't search same inode */
			append(&dir->dirlist, n);
		}

		if (link) {
			n->orig_link = link;
			curroffset = alignnode(n, curroffset, 0) + spaceneeded(n);
			continue;
		}

		if (S_ISREG(sb->st_mode)) {
			curroffset = alignnode(n, curroffset, spaceneeded(n));
			n->size = sb->st_size;
		}
#if 0
else
			curroffset = alignnode(n, curroffset, 0);
		if (S_ISLNK(sb->st_mode)) {
			n->size = sb->st_size;
		}
#endif
		curroffset += spaceneeded(n);

#if 0
		if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode)) {
			n->devnode = sb->st_rdev;
		}
#endif

		if (S_ISDIR(sb->st_mode)) {
			if (!strcmp(n->name, "..")) {
				curroffset = processdir(level+1, dir->realname, dp->d_name,
							sb, dir, root, curroffset);
			} else {
				curroffset = processdir(level+1, n->realname, dp->d_name,
							sb, n, root, curroffset);
			}
		}
	}
	closedir(dirfd);
	return curroffset;
}

void showhelp(const char *argv0)
{
	printf("genromfs %s\n","0.5");
	printf("Usage: %s [OPTIONS] -f IMAGE\n",argv0);
	printf("Create a romfs filesystem image from a directory\n");
	printf("\n");
	printf("  -f IMAGE               Output the image into this file\n");
	printf("  -d DIRECTORY           Use this directory as source\n");
	printf("  -v                     (Too) verbose operation\n");
	printf("  -V VOLUME              Use the specified volume name\n");
	printf("  -h                     Show this help\n");
	printf("\n");
}

int main(int argc, char *argv[])
{
	int c;
	char *dir = ".";
	char *outf = NULL;
	char *volname = NULL;
	int verbose=0;
	char buf[256];
	struct filenode *root;
	struct stat sb;
	int lastoff;
	FILE *f;

	while ((c = getopt(argc, argv, "V:vd:f:h")) != EOF) {
		switch(c) {
		case 'd':
			dir = optarg;
			break;
		case 'f':
			outf = optarg;
			break;
		case 'V':
			volname = optarg;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'h':
			showhelp(argv[0]);
			exit(0);
		default:
			exit(1);
		}
	}

	if (!volname) {
		sprintf(buf, "rom %08lx", (long unsigned int)time(NULL));
		volname = buf;
	}
	if (!outf) {
		fprintf(stderr, "%s: you must specify the destination file\n", argv[0]);
		fprintf(stderr, "Try `%s -h' for more information\n",argv[0]);
		exit(1);
	}
	if (strcmp(outf, "-") == 0) {
		f = fdopen(1,"wb");
	} else
		f = fopen(outf, "wb");

	if (!f) {
		perror(outf);
		exit(1);
	}

	realbase = strlen(dir);
	root = newnode(dir, volname, 0);
	root->parent = root;
	lastoff = processdir (1, dir, dir, &sb, root, root, spaceneeded(root));
	if (verbose)
		shownode(0, root, stderr);
	dumpall(root, lastoff, f);

	exit(0);
}
