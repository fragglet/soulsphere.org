/* 
   Copyright (C) 2003 Simon D. Howard

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   
   The GNU C library (glibc) includes a "cookies" feature, which allows 
   the normal file IO functions to be hooked into with user-specified
   callback functions.  This links libc with GnomeVFS, the Gnome 
   virtual filesystem.  In this way, any file accessible via GnomeVFS
   can be accessed using the standard C file API.

   Author: Simon D. Howard <sdh300@zepler.net>
 */

#define __USE_GNU
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <libio.h>
#include <errno.h>
#include <glib.h>

#include <libgnomevfs/gnome-vfs.h>

static int gnome_vfs_result_to_errno(GnomeVFSResult result)
{
	switch (result) {
	case GNOME_VFS_ERROR_ACCESS_DENIED:
		return EACCES;
	case GNOME_VFS_ERROR_DIRECTORY_BUSY:
		return EBUSY;
	case GNOME_VFS_ERROR_BAD_FILE:
		return EBADF;
	case GNOME_VFS_ERROR_FILE_EXISTS:
		return EEXIST;
	case GNOME_VFS_ERROR_INTERNAL:
		return EFAULT;
	case GNOME_VFS_ERROR_TOO_BIG:
		return EFBIG;
	case GNOME_VFS_ERROR_INTERRUPTED:
		return EINTR;
	case GNOME_VFS_ERROR_BAD_PARAMETERS:
		return EINVAL;
	case GNOME_VFS_ERROR_IO:
		return EIO;
	case GNOME_VFS_ERROR_IS_DIRECTORY:
		return EISDIR;
	case GNOME_VFS_ERROR_LOOP:
		return ELOOP;
	case GNOME_VFS_ERROR_TOO_MANY_LINKS:
		return EMLINK;
	case GNOME_VFS_ERROR_TOO_MANY_OPEN_FILES:
		return EMFILE;
	case GNOME_VFS_ERROR_DIRECTORY_NOT_EMPTY:
		return ENOTEMPTY;
	case GNOME_VFS_ERROR_NOT_FOUND:
		return ENOENT;
	case GNOME_VFS_ERROR_NO_MEMORY:
		return ENOMEM;
	case GNOME_VFS_ERROR_NO_SPACE:
		return ENOSPC;
	case GNOME_VFS_ERROR_NOT_A_DIRECTORY:
		return ENOTDIR;
	case GNOME_VFS_ERROR_NOT_PERMITTED:
		return EPERM;
	case GNOME_VFS_ERROR_READ_ONLY_FILE_SYSTEM:
		return EROFS;
	case GNOME_VFS_ERROR_NOT_SAME_FILE_SYSTEM:
		return EXDEV;
	case GNOME_VFS_ERROR_EOF:
		return 0;
	default:
		return EIO;
	}
}

static GnomeVFSSeekPosition libc_to_seek_position(int w)
{
	switch (w) {
	case SEEK_SET:
		return GNOME_VFS_SEEK_START;
	case SEEK_CUR:
		return GNOME_VFS_SEEK_CURRENT;
	case SEEK_END:
		return GNOME_VFS_SEEK_END;
	default:
		return -1;
	}
}

struct vfs_cookie {
	GnomeVFSHandle *handle;
	FILE *fs;
};

static size_t gnome_vfs_cookie_read(struct vfs_cookie *cookie,
				    void *buf, int len)
{
	GnomeVFSFileSize read_bytes;
	GnomeVFSResult result;

	result = gnome_vfs_read(cookie->handle, buf, len, &read_bytes);
	
	if (result == GNOME_VFS_ERROR_EOF) {
		cookie->fs->_flags |= _IO_EOF_SEEN;
		return -1;
	}
	
	if (result) {
		errno = gnome_vfs_result_to_errno(result);

		return -1;
	}

	return read_bytes;
}

static size_t gnome_vfs_cookie_write(struct vfs_cookie *cookie,
				     void *buf, int len)
{
	GnomeVFSFileSize wrote_bytes;
	GnomeVFSResult result;

	result = gnome_vfs_read(cookie->handle, buf, len, &wrote_bytes);
	
	if (result) {
		errno = gnome_vfs_result_to_errno(result);
		return -1;
	}

	return wrote_bytes;
}

static int gnome_vfs_cookie_seek(struct vfs_cookie *cookie,
				 _IO_off64_t *__pos, int __w)
{
	GnomeVFSResult result;
	GnomeVFSFileSize pos;

	result = gnome_vfs_seek(cookie->handle, 
				*__pos,
				libc_to_seek_position(__w));

	if (result) {
		errno = gnome_vfs_result_to_errno(result);
		return -1;
	}

	result = gnome_vfs_tell(cookie->handle, &pos);

	if (result) {
		errno = gnome_vfs_result_to_errno(result);
		return -1;
	}

	*__pos = pos;

	return 0;
}

static int gnome_vfs_cookie_close(struct vfs_cookie *cookie)
{
	gnome_vfs_close(cookie->handle);

	free(cookie);

	return 0;
}

static cookie_io_functions_t cookie_functions = {
	.read = (cookie_read_function_t *) gnome_vfs_cookie_read,
	.write = (cookie_write_function_t *) gnome_vfs_cookie_write,
	.seek = (cookie_seek_function_t *) gnome_vfs_cookie_seek,
	.close = (cookie_close_function_t *) gnome_vfs_cookie_close,
};

FILE *gnome_vfs_fopen_uri(GnomeVFSURI *uri, gchar *opentype)
{
	struct vfs_cookie *cookie;
	GnomeVFSHandle *handle;
	GnomeVFSResult result;
	GnomeVFSOpenMode vfs_opentype;
	FILE *fs;
	char *s;

	vfs_opentype = GNOME_VFS_OPEN_NONE;

	for (s=opentype; *s; ++s) {
		switch (*s) {
		case 'r':
			vfs_opentype |= GNOME_VFS_OPEN_READ;
			break;
		case 'a':
		case 'w':
			vfs_opentype |= GNOME_VFS_OPEN_WRITE;
			break;
		case '+':
			vfs_opentype |= GNOME_VFS_OPEN_READ;
			vfs_opentype |= GNOME_VFS_OPEN_WRITE;
			break;
		}
	}	

	result = gnome_vfs_open_uri(&handle, uri, vfs_opentype);

	if (result) {
		errno = gnome_vfs_result_to_errno(result);
		return NULL;
	}

	cookie = g_new0(struct vfs_cookie, 1);

	cookie->handle = handle;

	fs = fopencookie(cookie, opentype, cookie_functions);

	cookie->fs = fs;

	return fs;
}

FILE *gnome_vfs_fopen(gchar *text_uri, gchar *opentype)
{
	GnomeVFSURI *uri;
	FILE *fs;

	uri = gnome_vfs_uri_new(text_uri);

	fs = gnome_vfs_fopen_uri(uri, opentype);
	
	gnome_vfs_uri_unref(uri);

	return fs;
}

int main(int argc, char *argv[])
{
	FILE *fs;
	char buf[512];

	gnome_vfs_init();

	fs = gnome_vfs_fopen("http://slashdot.org/", "r");

	if (!fs) {
		printf("couldnt open url\n");
		abort();
	}

	while (!feof(fs)) {
		if (fgets(buf, sizeof(buf)-1, fs)) 
			printf("%s", buf);
	}

	fclose(fs);

	return 0;
}

