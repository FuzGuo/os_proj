#include <asm/segment.h>
#include <errno.h>
#include <string.h>

char _myname[21]; /* 20 chars + '\0' */

int sys_iam(const char *name)
{
    char str[22]; /* 20 chars + '\0' + '\n' */
    int i = 0;
    do
    {
        str[i] = get_fs_byte(name + i);
    } while (i <= 22 && str[i++] != '\0'); 
    if (i > 21)
    {
        return -EINVAL;
    }
    else
    {
        strcpy(_myname, str);
    }
    return 0;
}
int sys_whoami(char *name, unsigned int size)
{
    int length = strlen(_myname);
    
    if (size < length)
    {
        return -EINVAL;
    }
    else
    {
        int i = 0;
        for (i = 0; i < length; i++)
        {
            put_fs_byte(_myname[i], name + i);
        }
    }
    // printk("%s\n", _myname);
    return 0;
}