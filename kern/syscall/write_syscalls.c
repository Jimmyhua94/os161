#include <types.h>
#include <vfs.h>
#include <vnode.h>
#include <syscall.h>
#include <current.h>
#include <thread.h>
#include <proc.h>
#include <copyinout.h>
#include <limits.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <lib.h>
#include <uio.h>


int sys___write(int fd, const void *buf, size_t nbytes, int32_t *retval){
    if(curproc->ft[fd] == NULL){
        return EBADF;
    }
    if(curproc->ft[fd]->flags == O_RDONLY){
        return EBADF;
    }
    
    int result;
    
    struct iovec *iov = kmalloc(sizeof(*iov));
    struct uio *u = kmalloc(sizeof(*u));
    uio_kinit(iov,u,(void *)buf,nbytes,curproc->ft[fd]->offset,UIO_WRITE);
    
    result = VOP_WRITE(curproc->ft[fd]->path,u);
    if (result){
        return result;
    }
    
    *retval = nbytes - u->uio_resid;
    struct handler* handle = curproc->ft[fd];
    handle->offset = handle->offset + *retval;

    kfree(iov);
    kfree(u);
    return 0;
}