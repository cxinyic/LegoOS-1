cmd_kernel/locking/built-in.o :=  ld  -r -o kernel/locking/built-in.o kernel/locking/rwsem.o kernel/locking/rwsem-xadd.o kernel/locking/semaphore.o 
