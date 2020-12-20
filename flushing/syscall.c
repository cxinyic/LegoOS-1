#include <lego/mm.h>
#include <lego/syscalls.h>
#include <processor/fs.h>
#include <processor/pgtable.h>
#include <processor/processor.h>
#include <processor/distvm.h>
#include <processor/zerofill.h>
#include <processor/dependency_track.h>

int nr_dp_info =0;
struct dp_vector* dp_info_list;
struct dp_vector* old_dirty_pages;
struct dp_vector* new_dirty_pages;
spinlock_t dp_spinlock = __SPIN_LOCK_UNLOCKED(dp_spinlock);


asmlinkage long sys_mmap_track(unsigned long addr, unsigned long len,
        unsigned long prot, unsigned long flags, 
        unsigned long fd, unsigned long off)
{
#ifdef CONFIG_COMP_PROCESSOR
	spin_lock(&dp_spinlock);
	struct p2m_mmap_struct payload;
	struct p2m_mmap_reply_struct reply;
	struct file *f = NULL;
	long ret_len, ret_addr;

	syscall_enter("addr:%#lx,len:%#lx,prot:%#lx,flags:%#lx,fd:%lu,off:%#lx\n",
		addr, len, prot, flags, fd, off);

	if (offset_in_page(off))
		return -EINVAL;
	if (!len)
		return -EINVAL;
	len = PAGE_ALIGN(len);
	if (!len)
		return -ENOMEM;
	/* overflowed? */
	if ((off + len) < off)
		return -EOVERFLOW;

	/* file-backed mmap? */
	if (!(flags & MAP_ANONYMOUS)) {
		f = fdget(fd);
		if (!f)
			return -EBADF;
		memcpy(payload.f_name, f->f_name, MAX_FILENAME_LENGTH);
	} else
		memset(payload.f_name, 0, MAX_FILENAME_LENGTH);

	payload.pid = current->tgid;
	payload.addr = addr;
	payload.len = len;
	payload.prot = prot;
	payload.flags = flags;
	payload.pgoff = off >> PAGE_SHIFT;

	ret_len = net_send_reply_timeout(current_memory_home_node(), P2M_MMAP,
			&payload, sizeof(payload), &reply, sizeof(reply),
			false, DEF_NET_TIMEOUT);

	mmap_debug("ret_addr:%#Lx", reply.ret_addr);

	if (likely(ret_len == sizeof(reply))) {
		if (likely(reply.ret == RET_OKAY))
			ret_addr = reply.ret_addr;
		else
			ret_addr = (s64)reply.ret;
	} else
		ret_addr = -EIO;

	/*
	 * If mmap() succeed, we setup additional
	 * vma array, anon_bitmap, prot_bitmap, and zerofill_bitmap
	 */
	if (likely(ret_addr > 0)) {
#ifdef CONFIG_DISTRIBUTED_VMA_PROCESSOR
		map_mnode_from_reply(current->mm, &reply.map);
#endif
		if (flags & MAP_ANONYMOUS)
			zerofill_set_range(current, ret_addr, len);
	}

	if (f)
		put_file(f);
	
	if(nr_dp_info == 0 && dp_info_list == NULL){
		dp_info_list = (struct dp_vector*)kmalloc(sizeof(struct dp_vector), GFP_KERNEL);
		dp_vector_new(dp_info_list,sizeof(struct dp_info));
	}

	if(old_dirty_pages == NULL){
		old_dirty_pages = (struct dp_vector*)kmalloc(sizeof(struct dp_vector), GFP_KERNEL);
		dp_vector_new(old_dirty_pages,sizeof(struct dp_idx));
	}

	if(new_dirty_pages == NULL){
		new_dirty_pages = (struct dp_vector*)kmalloc(sizeof(struct dp_vector), GFP_KERNEL);
		dp_vector_new(new_dirty_pages,sizeof(struct dp_idx));
	}

	struct dp_info curr_dp_info;
	curr_dp_info.user_pid = current->pid;
	curr_dp_info.user_mm = current->mm;
	curr_dp_info.addr = ret_addr;
	curr_dp_info.len = len;
	curr_dp_info.nr_pages = len/PAGE_SIZE;
	curr_dp_info.dirty_page_list = (int*)kmalloc_array(curr_dp_info.nr_pages, sizeof(int), GFP_KERNEL);
	curr_dp_info.dp_pages = (struct dp_vector*)kmalloc_array(curr_dp_info.nr_pages, sizeof(struct dp_vector), GFP_KERNEL);
    curr_dp_info.pcm_list = (struct pcache_meta**)kmalloc_array(curr_dp_info.nr_pages, sizeof(struct pcache_meta*), GFP_KERNEL);

	int i;
	for(i=0;i<curr_dp_info.nr_pages;i++){
		*(curr_dp_info.dirty_page_list+i) = 0;
		*(curr_dp_info.pcm_list+i) = NULL;
		dp_vector_new(curr_dp_info.dp_pages+i, sizeof(struct dp_idx));
	}
	dp_vector_pushback(dp_info_list,&curr_dp_info);
	nr_dp_info += 1;
	spin_unlock(&dp_spinlock);

	return ret_addr;
#else
    return -1;
#endif
}
asmlinkage long sys_munmap_track(unsigned long addr, size_t len)
{
#ifdef CONFIG_COMP_PROCESSOR
	spin_lock(&dp_spinlock);
	struct p2m_munmap_struct payload;
	struct p2m_munmap_reply_struct retbuf;
	long retlen;

	munmap_debug("release: [%#lx - %#lx]", addr, addr + len);

	if (offset_in_page(addr) || addr > TASK_SIZE || len > TASK_SIZE - addr)
		return -EINVAL;

	len = PAGE_ALIGN(len);
	if (!len)
		return -EINVAL;

	payload.pid = current->tgid;
	payload.addr = addr;
	payload.len = len;

	retlen = net_send_reply_timeout(current_memory_home_node(), P2M_MUNMAP,
			&payload, sizeof(payload), &retbuf, sizeof(retbuf),
			false, DEF_NET_TIMEOUT);

	if (unlikely(retlen != sizeof(retbuf))) {
		retbuf.ret = -EIO;
		return retbuf.ret;
	}

	/* Unmap emulated pgtable */
	if (likely(retbuf.ret == 0)) {
		release_pgtable(current, addr, addr + len);
#ifdef CONFIG_DISTRIBUTED_VMA_PROCESSOR
		map_mnode_from_reply(current->mm, &retbuf.map);
#endif
	} else
		pr_err("munmap() fail: %s\n", ret_to_string(retbuf.ret));


    int i=0;
	int position = -1;
	struct dp_info * tmp;
	for(i=0;i<nr_dp_info;i++){
		tmp = (struct dp_info*)dp_vector_Nth(dp_info_list,i);
		if(tmp->addr == addr){
			position = i;
			break;
		}
	}
	
	if(position >= 0){
		tmp->user_pid = -1;
		tmp->user_mm = NULL;
		kfree(tmp->dirty_page_list);
		for(i=0;i<tmp->nr_pages;i++){
			dp_vector_dispose(tmp->dp_pages+i);
		}
		kfree(tmp->dp_pages);
		dp_vector_delete(dp_info_list,position);
		nr_dp_info = nr_dp_info-1;
	}

	if(nr_dp_info == 0){
		dp_vector_dispose(dp_info_list);
		kfree(dp_info_list);
		dp_vector_dispose(old_dirty_pages);
		kfree(old_dirty_pages);
		dp_vector_dispose(new_dirty_pages);
		kfree(new_dirty_pages);
		dp_info_list = NULL;
		new_dirty_pages = NULL;
		old_dirty_pages = NULL;
		
	}
	spin_unlock(&dp_spinlock);
	return retbuf.ret;
#else
    return -1;
#endif
}