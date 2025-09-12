#include <asm/segment.h> /* 用于 get_fs_byte, put_fs_byte */
#include <errno.h>       /* 用于错误码 EINVAL */
#include <string.h>      /* 用于 strcpy, strlen */

char _myname[21];

int sys_iam(const char *name)
{
	char tmp[21]; /* 使用临时缓冲区以在验证成功前避免覆写 _myname */
	int i;

	/* 20个字符 + '\0' */
	for (i = 0; i < 21; i++) {
		/* 从用户空间拷贝一个字节，并检查是否为字符串结束符 */
		if ((tmp[i] = get_fs_byte(name + i)) == '\0') {
			strcpy(_myname, tmp); 
			return 0;
		}
	}
	return -EINVAL; // 太长了
}

int sys_whoami(char *name, unsigned int size)
{
	int i, len = strlen(_myname);

	if (size <= len)
		return -EINVAL;

	for (i = 0; i <= len; i++)
		put_fs_byte(_myname[i], name + i);

	return 0;
}