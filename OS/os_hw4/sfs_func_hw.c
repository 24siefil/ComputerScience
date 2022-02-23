//
// Simple FIle System
// Student Name : 신성우
// Student Number : B617065
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "sfs_types.h"
#include "sfs_func.h"
#include "sfs_disk.h"
#include "sfs.h"

void	dump_directory();
size_t	ft_strlcpy();

/* BIT operation Macros */
/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

#define ERR	-1

static struct sfs_super spb;					// superblock
static struct sfs_dir sd_cwd = { SFS_NOINO };	// current working directory

/*
** =============================================================================
** error handling, mount, unmount
** =============================================================================
*/

enum err_code
{
	IS_NOT_A_FILE = -10,
	IS_A_DIRECTORY,
	INVALID_ARGUMENT,
	DIRECTORY_NOT_EMPTY,
	ALREADY_EXISTS,
	NOT_A_DIRECTORY,
	NO_BLOCK_AVAILABLE,
	DIRECTORY_FULL,
	DUMMY,
	NO_SUCH_FILE_OR_DIRECTORY
};

void error_message(const char *message, const char *path, int error_code)
{
	switch (error_code)
	{
		case -1:
			printf("%s: %s: No such file or directory\n",message, path); return ;
		case -2:
			printf("%s: %s: Not a directory\n",message, path); return ;
		case -3:
			printf("%s: %s: Directory full\n",message, path); return ;
		case -4:
			printf("%s: %s: No block available\n",message, path); return ;
		case -5:
			printf("%s: %s: Not a directory\n",message, path); return ;
		case -6:
			printf("%s: %s: Already exists\n",message, path); return ;
		case -7:
			printf("%s: %s: Directory not empty\n",message, path); return ;
		case -8:
			printf("%s: %s: Invalid argument\n",message, path); return ;
		case -9:
			printf("%s: %s: Is a directory\n",message, path); return ;
		case -10:
			printf("%s: %s: Is not a file\n",message, path); return ;
		default:
			printf("unknown error code\n");
			return ;
	}
}

void sfs_mount(const char *path)
{
	if ( sd_cwd.sfd_ino !=  SFS_NOINO ) // umount
	{
		disk_close();
		printf("%s, unmounted\n", spb.sp_volname);
		bzero(&spb, sizeof(struct sfs_super));
		sd_cwd.sfd_ino = SFS_NOINO;
	}
	printf("Disk image: %s\n", path);
	disk_open(path);
	disk_read( &spb, SFS_SB_LOCATION );
	printf("Superblock magic: %x\n", spb.sp_magic);
	assert( spb.sp_magic == SFS_MAGIC );
	printf("Number of blocks: %d\n", spb.sp_nblocks);
	printf("Volume name: %s\n", spb.sp_volname);
	printf("%s, mounted\n", spb.sp_volname);
	sd_cwd.sfd_ino = 1; // init at root
	sd_cwd.sfd_name[0] = '/';
	sd_cwd.sfd_name[1] = '\0';
}

#define MAP_END (SFS_MAP_LOCATION + (spb.sp_nblocks / SFS_BLOCKBITS))

void sfs_umount(void)
{
	if ( sd_cwd.sfd_ino !=  SFS_NOINO ) // umount
	{
		disk_close();
		printf("%s, unmounted\n", spb.sp_volname);
		bzero(&spb, sizeof(struct sfs_super));
		sd_cwd.sfd_ino = SFS_NOINO;
	}
}

/*
** =============================================================================
** level one - list segments
** =============================================================================
*/

void	print_name_in_dir(struct sfs_inode *inode)
{
	int					i, j;
	struct sfs_dir		dir_entry[SFS_DENTRYPERBLOCK];
	struct sfs_inode	buf_inode;

	i = 0;
	while (i < SFS_NDIRECT)
	{
		if (inode->sfi_direct[i] == 0) // skip
		{
			++i;
			continue ;
		}
		j = 0;
		disk_read(dir_entry, inode->sfi_direct[i]);
		while (j < SFS_DENTRYPERBLOCK)
		{
			if (dir_entry[j].sfd_ino == 0) // skip
			{
				++j;
				continue ;
			}
			disk_read(&buf_inode, dir_entry[j].sfd_ino);
			if (buf_inode.sfi_type == SFS_TYPE_DIR)
				printf("%s/\t", dir_entry[j].sfd_name);
			else
				printf("%s\t", dir_entry[j].sfd_name);
			++j;
		}
		++i;
	}
	printf("\n");
}

void	sfs_ls(const char *path)
{
	int					i, j;
	bool				flag_find;
	struct sfs_inode	cwd_inode;
	struct sfs_inode	inode_to_ls;
	struct sfs_dir		dir_entry[SFS_DENTRYPERBLOCK];

	disk_read(&cwd_inode, sd_cwd.sfd_ino);
	flag_find = false;
	if (path == NULL) // cwd
	{
		print_name_in_dir(&cwd_inode);
		return ;
	}
	else // arbitrary dir
	{
		i = 0;
		while (i < SFS_NDIRECT)
		{
			j = 0;
			disk_read(dir_entry, cwd_inode.sfi_direct[i]);
			while (j < SFS_DENTRYPERBLOCK)
			{
				if (strcmp(dir_entry[j].sfd_name, path) == 0)
				{
					flag_find = true;
					break ;
				}
				++j;
			}
			if (flag_find == true)
			{
				disk_read(&inode_to_ls, dir_entry[j].sfd_ino);
				if (inode_to_ls.sfi_type == SFS_TYPE_DIR)
					print_name_in_dir(&inode_to_ls);
				else
					printf("%s\n", dir_entry[j].sfd_name);
				return ;
			}
			++i;
		}
		error_message("ls", path, NO_SUCH_FILE_OR_DIRECTORY);
	}
}

/*
** =============================================================================
** level one - change directory
** =============================================================================
*/

void	sfs_cd(const char *path) // update sd_cwd
{
	int					i, j;
	bool				flag_find;
	struct sfs_inode	cwd_inode;
	struct sfs_inode	inode_to_ch;
	struct sfs_dir		dir_entry[SFS_DENTRYPERBLOCK];

	if (path == NULL) // cwd <- root
	{
		sd_cwd.sfd_ino = SFS_ROOT_LOCATION;
		ft_strlcpy(sd_cwd.sfd_name, "/", SFS_NAMELEN);
		return ;
	}
	i = 0;
	disk_read(&cwd_inode, sd_cwd.sfd_ino);
	flag_find = false;
	while (i < SFS_NDIRECT)
	{
		j = 0;
		disk_read(dir_entry, cwd_inode.sfi_direct[i]);
		while (j < SFS_DENTRYPERBLOCK)
		{
			if (strcmp(dir_entry[j].sfd_name, path) == 0)
			{
				flag_find = true;
				break ;
			}
			++j;
		}
		if (flag_find == true)
		{
			disk_read(&inode_to_ch, dir_entry[j].sfd_ino);
			if (inode_to_ch.sfi_type == SFS_TYPE_FILE) // 해당 경로가 디렉토리가 아닌 경우
				error_message("cd", path, NOT_A_DIRECTORY);
			else
			{
				sd_cwd.sfd_ino = dir_entry[j].sfd_ino;
				ft_strlcpy(sd_cwd.sfd_name, path, SFS_NAMELEN);
			}
			return ;
		}
		++i;
	}
	error_message("cd", path, NO_SUCH_FILE_OR_DIRECTORY); // 디렉토리가 존재하지 않는 경우
}

/*
** =============================================================================
** level two - make directory
** =============================================================================
*/

bool	validate_path(const char *org_path, struct sfs_inode *cwd_inode, char *cmd)
{
	int				i, j;
	struct sfs_dir	dir_entry[SFS_DENTRYPERBLOCK];

	i = 0;
	while (i < SFS_NDIRECT)
	{
		j = 0;
		disk_read(dir_entry, cwd_inode->sfi_direct[i]);
		while (j < SFS_DENTRYPERBLOCK)
		{
			if (strcmp(dir_entry[j].sfd_name, org_path) == 0)
			{
				error_message(cmd, org_path, ALREADY_EXISTS);
				return (false);
			}
			++j;
		}
		++i;
	}
	return (true);
}

bool	validate_dir_entry(const char *org_path, struct sfs_inode *cwd_inode, char *cmd)
{
	int				i, j;
	struct sfs_dir	dir_entry[SFS_DENTRYPERBLOCK];

	i = 0;
	while (i < SFS_NDIRECT)
	{
		j = 0;
		disk_read(dir_entry, cwd_inode->sfi_direct[i]);
		while (j < SFS_DENTRYPERBLOCK)
		{
			if (dir_entry[j].sfd_ino == 0)
				return (true);
			++j;
		}
		++i;
	}
	error_message(cmd, org_path, DIRECTORY_FULL);
	return (false);
}

int	get_avail_bit_idx(char byte)
{
	int		i;

	i = 0;
	while (i < 8)
	{
		if (BIT_CHECK(byte, i) == false)
			return (i);
		++i;
	}
	return (ERR); // dummy
}

int	get_new_blk_nbr(void)
{
	int		i, j;
	int		avail_bit_idx;
	int		new_blk_nbr;
	char	bitmap[SFS_BLOCKSIZE];
	char	full = 0b11111111;

	i = SFS_MAP_LOCATION;
	while (i <= MAP_END)
	{
		j = 0;
		disk_read(bitmap, i);
		while (j < SFS_BLOCKSIZE)
		{
			if (bitmap[j] != full)
			{
				avail_bit_idx = get_avail_bit_idx(bitmap[j]); // reversed idx!
				BIT_SET(bitmap[j], avail_bit_idx);
				disk_write(bitmap, i); // i == blk nbr
				new_blk_nbr = SFS_BLOCKBITS * (i - SFS_MAP_LOCATION) + CHAR_BIT * j + avail_bit_idx;
				return (new_blk_nbr);
			}
			++j;
		}
		++i;
	}
	return (ERR);
}

void	sfs_mkdir(const char *org_path)
{
	int					i, j;
	int					new_blk_nbr_1, new_blk_nbr_2, new_blk_nbr_3;
	struct sfs_inode	cwd_inode;
	struct sfs_inode 	new_inode;
	struct sfs_dir 		dir_entry[SFS_DENTRYPERBLOCK];
	struct sfs_dir		new_dir_entry[SFS_DENTRYPERBLOCK];

	disk_read(&cwd_inode, sd_cwd.sfd_ino);
	if (validate_path(org_path, &cwd_inode, "mkdir") == false || \
		validate_dir_entry(org_path, &cwd_inode, "mkdir") == false) // path가 이미 있는 경우(같은 이름의 파일이나 디렉토리) // 할당 받을 디렉토리 엔트리가 없는 경우
		return ;
	if ((new_blk_nbr_1 = get_new_blk_nbr()) == ERR || \
		(new_blk_nbr_2 = get_new_blk_nbr()) == ERR) // 할당할 블록이 없는 경우
	{
		error_message("mkdir", org_path, NO_BLOCK_AVAILABLE);
		return ;
	}
	i = 0;
	while (i < SFS_NDIRECT)
	{
		if (cwd_inode.sfi_direct[i] == 0) // in this case, need one more blk (total of 3)
		{
			if ((new_blk_nbr_3 = get_new_blk_nbr()) == ERR)
			{
				error_message("mkdir", org_path, NO_BLOCK_AVAILABLE);
				return ;
			}
			// update cwd_inode, dir entry (create)
			cwd_inode.sfi_size += sizeof(struct sfs_dir);
			cwd_inode.sfi_direct[i] = new_blk_nbr_1;
			disk_write(&cwd_inode, sd_cwd.sfd_ino);
			bzero(dir_entry, sizeof(struct sfs_dir) * SFS_DENTRYPERBLOCK); // SFS_NOINO
			dir_entry[0].sfd_ino = new_blk_nbr_2;
			ft_strlcpy(dir_entry[0].sfd_name, org_path, SFS_NAMELEN);
			disk_write(dir_entry, cwd_inode.sfi_direct[i]);

			// update new inode
			bzero(&new_inode, SFS_BLOCKSIZE); // SFS_NOINO
			new_inode.sfi_size += sizeof(struct sfs_dir) * 2;
			new_inode.sfi_type = SFS_TYPE_DIR;
			new_inode.sfi_direct[0] = new_blk_nbr_3;
			disk_write(&new_inode, new_blk_nbr_2);

			// update new dir entry
			bzero(new_dir_entry, sizeof(struct sfs_dir) * SFS_DENTRYPERBLOCK); // SFS_NOINO
			new_dir_entry[0].sfd_ino = new_blk_nbr_2;
			ft_strlcpy(new_dir_entry[0].sfd_name, ".", SFS_NAMELEN);
			new_dir_entry[1].sfd_ino = sd_cwd.sfd_ino;
			ft_strlcpy(new_dir_entry[1].sfd_name, "..", SFS_NAMELEN);
			disk_write(new_dir_entry, new_blk_nbr_3);
			return ;
		}
		else
		{
			j = 0;
			disk_read(dir_entry, cwd_inode.sfi_direct[i]);
			while (j < SFS_DENTRYPERBLOCK)
			{
				if (dir_entry[j].sfd_ino == 0)
				{
					// update cwd_inode, dir entry
					cwd_inode.sfi_size += sizeof(struct sfs_dir);
					disk_write(&cwd_inode, sd_cwd.sfd_ino);
					dir_entry[j].sfd_ino = new_blk_nbr_1;
					ft_strlcpy(dir_entry[j].sfd_name, org_path, SFS_NAMELEN);
					disk_write(dir_entry, cwd_inode.sfi_direct[i]);

					// update new inode
					bzero(&new_inode, SFS_BLOCKSIZE); // SFS_NOINO
					new_inode.sfi_size += sizeof(struct sfs_dir) * 2;
					new_inode.sfi_type = SFS_TYPE_DIR;
					new_inode.sfi_direct[0] = new_blk_nbr_2;
					disk_write(&new_inode, new_blk_nbr_1);

					// update new dir entry
					bzero(new_dir_entry, sizeof(struct sfs_dir) * SFS_DENTRYPERBLOCK); // SFS_NOINO
					new_dir_entry[0].sfd_ino = new_blk_nbr_1;
					ft_strlcpy(new_dir_entry[0].sfd_name, ".", SFS_NAMELEN);
					new_dir_entry[1].sfd_ino = sd_cwd.sfd_ino;
					ft_strlcpy(new_dir_entry[1].sfd_name, "..", SFS_NAMELEN);
					disk_write(new_dir_entry, new_blk_nbr_2);
					return ;
				}
				++j;
			}
		}
		++i;
	}
}

/*
** =============================================================================
** level two - remove directory
** =============================================================================
*/

void	sfs_rmdir(const char* org_path)
{
	int					i, j, k;
	int					byte_idx, bit_idx, bitmap_idx, byte_idx_in_bitmap;
	char				bitmap[SFS_BLOCKSIZE];
	bool				flag_find;
	struct sfs_inode	cwd_inode;
	struct sfs_inode	inode_to_rm;
	struct sfs_dir		dir_entry[SFS_DENTRYPERBLOCK];

	if (strcmp(org_path, ".") == 0)
	{
		error_message("rmdir", org_path, INVALID_ARGUMENT);
		return ;
	}
	i = 0;
	disk_read(&cwd_inode, sd_cwd.sfd_ino);
	flag_find = false;
	while (i < SFS_NDIRECT)
	{
		j = 0;
		disk_read(dir_entry, cwd_inode.sfi_direct[i]);
		while (j < SFS_DENTRYPERBLOCK)
		{
			if (strcmp(dir_entry[j].sfd_name, org_path) == 0)
			{
				flag_find = true;
				break ;
			}
			++j;
		}
		if (flag_find == true)
		{
			disk_read(&inode_to_rm, dir_entry[j].sfd_ino);
			if (inode_to_rm.sfi_type == SFS_TYPE_FILE)
			{
				error_message("rmdir", org_path, NOT_A_DIRECTORY); // 해당 경로가 디렉토리가 아닌경우
				return ;
			}
			else
			{
				if ((size_t)inode_to_rm.sfi_size > sizeof(struct sfs_dir) * 2) // 디렉토리가 비어있지 않은 경우
				{
					error_message("rmdir", org_path, DIRECTORY_NOT_EMPTY);
					return ;
				}
				// rm data
				k = 0;
				while (k < SFS_NDIRECT)
				{
					if (inode_to_rm.sfi_direct[k] != SFS_NOINO)
					{
						byte_idx = inode_to_rm.sfi_direct[k] / CHAR_BIT;
						bit_idx = inode_to_rm.sfi_direct[k] - byte_idx * CHAR_BIT;
						bitmap_idx = SFS_MAP_LOCATION + byte_idx / SFS_BLOCKSIZE;
						disk_read(bitmap, bitmap_idx);
						byte_idx_in_bitmap = byte_idx - SFS_BLOCKSIZE * (byte_idx / SFS_BLOCKSIZE);
						BIT_FLIP(bitmap[byte_idx_in_bitmap], bit_idx);
						disk_write(bitmap, bitmap_idx);
					}
					++k;
				}

				// rm inode
				byte_idx = dir_entry[j].sfd_ino / CHAR_BIT;
				bit_idx = dir_entry[j].sfd_ino - byte_idx * CHAR_BIT;
				bitmap_idx = SFS_MAP_LOCATION + byte_idx / SFS_BLOCKSIZE;
				disk_read(bitmap, bitmap_idx);
				byte_idx_in_bitmap = byte_idx - SFS_BLOCKSIZE * (byte_idx / SFS_BLOCKSIZE);
				BIT_FLIP(bitmap[byte_idx_in_bitmap], bit_idx);
				disk_write(bitmap, bitmap_idx);

				// update dir entry, inode
				bzero(&dir_entry[j], sizeof(struct sfs_dir)); // SFS_NOINO
				disk_write(dir_entry, cwd_inode.sfi_direct[i]);
				cwd_inode.sfi_size -= sizeof(struct sfs_dir);
				disk_write(&cwd_inode, sd_cwd.sfd_ino);
				return ;
			}
		}
		++i;
	}
	error_message("rmdir", org_path, NO_SUCH_FILE_OR_DIRECTORY); // path가 없는 경우
}

/*
** =============================================================================
** level two - remove
** =============================================================================
*/

void	sfs_rm(const char *path)
{
	int					i, j, k;
	int					byte_idx, bit_idx, bitmap_idx, byte_idx_in_bitmap;
	char				bitmap[SFS_BLOCKSIZE];
	bool				flag_find;
	struct sfs_inode	cwd_inode;
	struct sfs_inode	inode_to_rm;
	struct sfs_dir		dir_entry[SFS_DENTRYPERBLOCK];

	i = 0;
	disk_read(&cwd_inode, sd_cwd.sfd_ino);
	flag_find = false;
	while (i < SFS_NDIRECT)
	{
		j = 0;
		disk_read(dir_entry, cwd_inode.sfi_direct[i]);
		while (j < SFS_DENTRYPERBLOCK)
		{
			if (strcmp(dir_entry[j].sfd_name, path) == 0)
			{
				flag_find = true;
				break ;
			}
			++j;
		}
		if (flag_find == true)
		{
			disk_read(&inode_to_rm, dir_entry[j].sfd_ino);
			if (inode_to_rm.sfi_type == SFS_TYPE_DIR) // 해당경로가 실제 데이터 파일인지
			{
				error_message("rm", path, IS_A_DIRECTORY);
				return ;
			}
			else
			{
				// rm data
				k = 0;
				while (k < SFS_NDIRECT)
				{
					if (inode_to_rm.sfi_direct[k] != SFS_NOINO)
					{
						byte_idx = inode_to_rm.sfi_direct[k] / CHAR_BIT;
						bit_idx = inode_to_rm.sfi_direct[k] - byte_idx * CHAR_BIT;
						bitmap_idx = SFS_MAP_LOCATION + byte_idx / SFS_BLOCKSIZE;
						disk_read(bitmap, bitmap_idx);
						byte_idx_in_bitmap = byte_idx - SFS_BLOCKSIZE * (byte_idx / SFS_BLOCKSIZE);
						BIT_FLIP(bitmap[byte_idx_in_bitmap], bit_idx);
						disk_write(bitmap, bitmap_idx);
					}
					++k;
				}

				// rm inode
				byte_idx = dir_entry[j].sfd_ino / CHAR_BIT;
				bit_idx = dir_entry[j].sfd_ino - byte_idx * CHAR_BIT;
				bitmap_idx = SFS_MAP_LOCATION + byte_idx / SFS_BLOCKSIZE;
				disk_read(bitmap, bitmap_idx);
				byte_idx_in_bitmap = byte_idx - SFS_BLOCKSIZE * (byte_idx / SFS_BLOCKSIZE);
				BIT_FLIP(bitmap[byte_idx_in_bitmap], bit_idx);
				disk_write(bitmap, bitmap_idx);

				// update dir entry, inode
				bzero(&dir_entry[j], sizeof(struct sfs_dir)); // SFS_NOINO
				disk_write(dir_entry, cwd_inode.sfi_direct[i]);
				cwd_inode.sfi_size -= sizeof(struct sfs_dir);
				disk_write(&cwd_inode, sd_cwd.sfd_ino);
				return ;
			}
		}
		++i;
	}
	error_message("rm", path, NO_SUCH_FILE_OR_DIRECTORY); // path가 실제로 존재하는지
}

/*
** =============================================================================
** level two - touch
** =============================================================================
*/

void sfs_touch(const char *path)
{
	int					i, j, new_blk_nbr;
	struct sfs_inode	cwd_inode;
	struct sfs_inode	new_inode;
	struct sfs_dir		dir_entry[SFS_DENTRYPERBLOCK];

	disk_read(&cwd_inode, sd_cwd.sfd_ino);
	if ((validate_path(path, &cwd_inode, "touch") == false) || \
		validate_dir_entry(path, &cwd_inode, "touch") == false) // path가 이미 존재하는 경우 // 디렉토리가 꽉차서 더이상 엔트리를 허용 못하는 경우
		return ;
	i = 0;
	while (i < SFS_NDIRECT)
	{
		j = 0;
		disk_read(dir_entry, cwd_inode.sfi_direct[i]);
		while (j < SFS_DENTRYPERBLOCK)
		{
			if (dir_entry[j].sfd_ino == 0)
			{
				// update cwd_inode
				cwd_inode.sfi_size += sizeof(struct sfs_dir);
				disk_write(&cwd_inode, sd_cwd.sfd_ino);

				// update dir entry
				if ((new_blk_nbr = get_new_blk_nbr()) == ERR) // I-node를 할당 못하는 경우
				{
					error_message("touch", path, NO_BLOCK_AVAILABLE);
					return ;
				}
				dir_entry[j].sfd_ino = new_blk_nbr;
				ft_strlcpy(dir_entry[j].sfd_name, path, SFS_NAMELEN);
				disk_write(dir_entry, cwd_inode.sfi_direct[i]);

				// create new inode (empty)
				bzero(&new_inode, SFS_BLOCKSIZE); // SFS_NOINO
				new_inode.sfi_type = SFS_TYPE_FILE;
				disk_write(&new_inode, new_blk_nbr);
				return ;
			}
			++j;
		}
		++i;
	}
	error_message("touch", path, DIRECTORY_FULL);
}

/*
** =============================================================================
** level two - move
** =============================================================================
*/

void	sfs_mv(const char *src_name, const char *dst_name)
{
	int					i, j;
	struct sfs_inode	cwd_inode;
	struct sfs_dir		dir_entry[SFS_DENTRYPERBLOCK];

	if (strcmp(src_name, ".") == 0 || strcmp(src_name, "..") == 0)
	{
		error_message("mv", src_name, INVALID_ARGUMENT);
		return ;
	}
	if (strcmp(dst_name, ".") == 0 || strcmp(dst_name, "..") == 0)
	{
		error_message("mv", dst_name, INVALID_ARGUMENT);
		return ;
	}
	i = 0;
	disk_read(&cwd_inode, sd_cwd.sfd_ino);
	while (i < SFS_NDIRECT)
	{
		j = 0;
		disk_read(dir_entry, cwd_inode.sfi_direct[i]);
		while (j < SFS_DENTRYPERBLOCK)
		{
			if (strcmp(dir_entry[j].sfd_name, dst_name) == 0) // path2가 이미 존재하는 경우
			{
				error_message("mv", dst_name, ALREADY_EXISTS);
				return ;
			}
			++j;
		}
		++i;
	}
	i = 0;
	while (i < SFS_NDIRECT)
	{
		j = 0;
		disk_read(dir_entry, cwd_inode.sfi_direct[i]);
		while (j < SFS_DENTRYPERBLOCK)
		{
			if (strcmp(dir_entry[j].sfd_name, src_name) == 0)
			{
				ft_strlcpy(dir_entry[j].sfd_name, dst_name, SFS_NAMELEN);
				disk_write(dir_entry, cwd_inode.sfi_direct[i]);
				return ;
			}
			++j;
		}
		++i;
	}
	error_message("mv", src_name, NO_SUCH_FILE_OR_DIRECTORY); // path1이 존재하지 않는 경우
}

/*
** =============================================================================
** dump, libft
** =============================================================================
*/

void sfs_cpin(const char* local_path, const char* path)
{
	printf("Not Implemented\n");
}

void sfs_cpout(const char* local_path, const char* path)
{
	printf("Not Implemented\n");
}

void dump_inode(struct sfs_inode inode)
{
	int i;
	struct sfs_dir dir_entry[SFS_DENTRYPERBLOCK];

	printf("size %d type %d direct ", inode.sfi_size, inode.sfi_type);
	for (i = 0; i < SFS_NDIRECT; ++i)
	{
		printf(" %d ", inode.sfi_direct[i]);
	}
	printf(" indirect %d",inode.sfi_indirect);
	printf("\n");
	if (inode.sfi_type == SFS_TYPE_DIR)
	{
		for(i = 0; i < SFS_NDIRECT; ++i)
		{
			if (inode.sfi_direct[i] == 0)
				break ;
			disk_read(dir_entry, inode.sfi_direct[i]);
			dump_directory(dir_entry);
		}
	}
}

void dump_directory(struct sfs_dir dir_entry[])
{
	int i;
	struct sfs_inode inode;

	for (i = 0; i < SFS_DENTRYPERBLOCK; ++i)
	{
		printf("%d %s\n",dir_entry[i].sfd_ino, dir_entry[i].sfd_name);
		disk_read(&inode,dir_entry[i].sfd_ino);
		if (inode.sfi_type == SFS_TYPE_FILE)
		{
			printf("\t");
			dump_inode(inode);
		}
	}
}

void sfs_dump(void) // dump the current directory structure
{
	struct sfs_inode c_inode;

	disk_read(&c_inode, sd_cwd.sfd_ino);
	printf("cwd inode %d name %s\n",sd_cwd.sfd_ino,sd_cwd.sfd_name);
	dump_inode(c_inode);
	printf("\n");
}

size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize)
{
	size_t	i;

	if (!src)
		return (0);
	if (dstsize == 0)
		return (strlen(src));
	i = 0;
	while (i < dstsize - 1 && src[i])
	{
		dst[i] = src[i];
		++i;
	}
	dst[i] = '\0';
	return (strlen(src));
}
