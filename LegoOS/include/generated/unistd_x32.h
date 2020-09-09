#ifndef _ASM_X86_UNISTD_X32_H
#define _ASM_X86_UNISTD_X32_H 1

#define __NR_read (__X32_SYSCALL_BIT + 0)
#define __NR_write (__X32_SYSCALL_BIT + 1)
#define __NR_open (__X32_SYSCALL_BIT + 2)
#define __NR_close (__X32_SYSCALL_BIT + 3)
#define __NR_stat (__X32_SYSCALL_BIT + 4)
#define __NR_fstat (__X32_SYSCALL_BIT + 5)
#define __NR_lstat (__X32_SYSCALL_BIT + 6)
#define __NR_poll (__X32_SYSCALL_BIT + 7)
#define __NR_lseek (__X32_SYSCALL_BIT + 8)
#define __NR_mmap (__X32_SYSCALL_BIT + 9)
#define __NR_mprotect (__X32_SYSCALL_BIT + 10)
#define __NR_munmap (__X32_SYSCALL_BIT + 11)
#define __NR_brk (__X32_SYSCALL_BIT + 12)
#define __NR_rt_sigprocmask (__X32_SYSCALL_BIT + 14)
#define __NR_pread64 (__X32_SYSCALL_BIT + 17)
#define __NR_pwrite64 (__X32_SYSCALL_BIT + 18)
#define __NR_access (__X32_SYSCALL_BIT + 21)
#define __NR_pipe (__X32_SYSCALL_BIT + 22)
#define __NR_sched_yield (__X32_SYSCALL_BIT + 24)
#define __NR_mremap (__X32_SYSCALL_BIT + 25)
#define __NR_msync (__X32_SYSCALL_BIT + 26)
#define __NR_madvise (__X32_SYSCALL_BIT + 28)
#define __NR_dup (__X32_SYSCALL_BIT + 32)
#define __NR_dup2 (__X32_SYSCALL_BIT + 33)
#define __NR_pause (__X32_SYSCALL_BIT + 34)
#define __NR_nanosleep (__X32_SYSCALL_BIT + 35)
#define __NR_setitimer (__X32_SYSCALL_BIT + 38)
#define __NR_getpid (__X32_SYSCALL_BIT + 39)
#define __NR_socket (__X32_SYSCALL_BIT + 41)
#define __NR_connect (__X32_SYSCALL_BIT + 42)
#define __NR_accept (__X32_SYSCALL_BIT + 43)
#define __NR_sendto (__X32_SYSCALL_BIT + 44)
#define __NR_bind (__X32_SYSCALL_BIT + 49)
#define __NR_listen (__X32_SYSCALL_BIT + 50)
#define __NR_clone (__X32_SYSCALL_BIT + 56)
#define __NR_fork (__X32_SYSCALL_BIT + 57)
#define __NR_vfork (__X32_SYSCALL_BIT + 58)
#define __NR_exit (__X32_SYSCALL_BIT + 60)
#define __NR_wait4 (__X32_SYSCALL_BIT + 61)
#define __NR_kill (__X32_SYSCALL_BIT + 62)
#define __NR_uname (__X32_SYSCALL_BIT + 63)
#define __NR_fcntl (__X32_SYSCALL_BIT + 72)
#define __NR_fsync (__X32_SYSCALL_BIT + 74)
#define __NR_truncate (__X32_SYSCALL_BIT + 76)
#define __NR_ftruncate (__X32_SYSCALL_BIT + 77)
#define __NR_getdents (__X32_SYSCALL_BIT + 78)
#define __NR_getcwd (__X32_SYSCALL_BIT + 79)
#define __NR_chdir (__X32_SYSCALL_BIT + 80)
#define __NR_rename (__X32_SYSCALL_BIT + 82)
#define __NR_mkdir (__X32_SYSCALL_BIT + 83)
#define __NR_rmdir (__X32_SYSCALL_BIT + 84)
#define __NR_creat (__X32_SYSCALL_BIT + 85)
#define __NR_unlink (__X32_SYSCALL_BIT + 87)
#define __NR_readlink (__X32_SYSCALL_BIT + 89)
#define __NR_gettimeofday (__X32_SYSCALL_BIT + 96)
#define __NR_getrlimit (__X32_SYSCALL_BIT + 97)
#define __NR_getrusage (__X32_SYSCALL_BIT + 98)
#define __NR_sysinfo (__X32_SYSCALL_BIT + 99)
#define __NR_getuid (__X32_SYSCALL_BIT + 102)
#define __NR_getgid (__X32_SYSCALL_BIT + 104)
#define __NR_setuid (__X32_SYSCALL_BIT + 105)
#define __NR_setgid (__X32_SYSCALL_BIT + 106)
#define __NR_geteuid (__X32_SYSCALL_BIT + 107)
#define __NR_getegid (__X32_SYSCALL_BIT + 108)
#define __NR_getppid (__X32_SYSCALL_BIT + 110)
#define __NR_getpgrp (__X32_SYSCALL_BIT + 111)
#define __NR_rt_sigsuspend (__X32_SYSCALL_BIT + 130)
#define __NR_statfs (__X32_SYSCALL_BIT + 137)
#define __NR_prctl (__X32_SYSCALL_BIT + 157)
#define __NR_arch_prctl (__X32_SYSCALL_BIT + 158)
#define __NR_setrlimit (__X32_SYSCALL_BIT + 160)
#define __NR_sync (__X32_SYSCALL_BIT + 162)
#define __NR_gettid (__X32_SYSCALL_BIT + 186)
#define __NR_tkill (__X32_SYSCALL_BIT + 200)
#define __NR_time (__X32_SYSCALL_BIT + 201)
#define __NR_futex (__X32_SYSCALL_BIT + 202)
#define __NR_sched_setaffinity (__X32_SYSCALL_BIT + 203)
#define __NR_sched_getaffinity (__X32_SYSCALL_BIT + 204)
#define __NR_epoll_create (__X32_SYSCALL_BIT + 213)
#define __NR_set_tid_address (__X32_SYSCALL_BIT + 218)
#define __NR_restart_syscall (__X32_SYSCALL_BIT + 219)
#define __NR_clock_gettime (__X32_SYSCALL_BIT + 228)
#define __NR_exit_group (__X32_SYSCALL_BIT + 231)
#define __NR_epoll_wait (__X32_SYSCALL_BIT + 232)
#define __NR_epoll_ctl (__X32_SYSCALL_BIT + 233)
#define __NR_tgkill (__X32_SYSCALL_BIT + 234)
#define __NR_mbind (__X32_SYSCALL_BIT + 237)
#define __NR_openat (__X32_SYSCALL_BIT + 257)
#define __NR_newfstatat (__X32_SYSCALL_BIT + 262)
#define __NR_unlinkat (__X32_SYSCALL_BIT + 263)
#define __NR_epoll_create1 (__X32_SYSCALL_BIT + 291)
#define __NR_pipe2 (__X32_SYSCALL_BIT + 293)
#define __NR_getcpu (__X32_SYSCALL_BIT + 309)
#define __NR_checkpoint_process (__X32_SYSCALL_BIT + 600)
#define __NR_pcache_stat (__X32_SYSCALL_BIT + 601)
#define __NR_drop_page_cache (__X32_SYSCALL_BIT + 611)

#endif /* _ASM_X86_UNISTD_X32_H */
