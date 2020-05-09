/*
 * Automatically generated C config: don't edit
 * Nautilus version: 
 * Sat May  9 22:32:56 2020
 */
#define AUTOCONF_INCLUDED

/*
 * Platform
 */
#define NAUT_CONFIG_X86_64_HOST 1
#undef NAUT_CONFIG_XEON_PHI
#undef NAUT_CONFIG_HVM_HRT
#undef NAUT_CONFIG_GEM5
#define NAUT_CONFIG_MAX_CPUS 256
#define NAUT_CONFIG_MAX_IOAPICS 16
#undef NAUT_CONFIG_PALACIOS

/*
 * Build
 */
#define NAUT_CONFIG_USE_NAUT_BUILTINS 1
#define NAUT_CONFIG_CXX_SUPPORT 1
#undef NAUT_CONFIG_RUST_SUPPORT
#define NAUT_CONFIG_USE_GCC 1
#undef NAUT_CONFIG_USE_CLANG
#undef NAUT_CONFIG_USE_WLLVM
#define NAUT_CONFIG_COMPILER_PREFIX ""
#define NAUT_CONFIG_COMPILER_SUFFIX ""
#define NAUT_CONFIG_TOOLCHAIN_ROOT ""

/*
 * Configuration
 */
#define NAUT_CONFIG_MAX_THREADS 1024
#undef NAUT_CONFIG_RUN_TESTS_AT_BOOT
#define NAUT_CONFIG_THREAD_EXIT_KEYCODE 196
#undef NAUT_CONFIG_USE_TICKETLOCKS
#undef NAUT_CONFIG_PARTITION_SUPPORT
#define NAUT_CONFIG_VIRTUAL_CONSOLE_DISPLAY_NAME 1
#undef NAUT_CONFIG_VIRTUAL_CONSOLE_CHARDEV_CONSOLE
#undef NAUT_CONFIG_VIRTUAL_CONSOLE_SERIAL_MIRROR

/*
 * Scheduler Options
 */
#define NAUT_CONFIG_UTILIZATION_LIMIT 99
#define NAUT_CONFIG_SPORADIC_RESERVATION 10
#define NAUT_CONFIG_APERIODIC_RESERVATION 10
#define NAUT_CONFIG_HZ 10
#define NAUT_CONFIG_INTERRUPT_REINJECTION_DELAY_NS 10000
#undef NAUT_CONFIG_AUTO_REAP
#undef NAUT_CONFIG_WORK_STEALING
#undef NAUT_CONFIG_TASK_IN_SCHED
#undef NAUT_CONFIG_TASK_THREAD
#undef NAUT_CONFIG_TASK_IN_IDLE
#undef NAUT_CONFIG_INTERRUPT_THREAD
#undef NAUT_CONFIG_APERIODIC_DYNAMIC_QUANTUM
#undef NAUT_CONFIG_APERIODIC_DYNAMIC_LIFETIME
#undef NAUT_CONFIG_APERIODIC_LOTTERY
#define NAUT_CONFIG_APERIODIC_ROUND_ROBIN 1

/*
 * Fiber Options
 */
#undef NAUT_CONFIG_FIBER_ENABLE
#undef NAUT_CONFIG_REAL_MODE_INTERFACE

/*
 * Watchdog Options
 */
#undef NAUT_CONFIG_WATCHDOG
#undef NAUT_CONFIG_ISOCORE
#undef NAUT_CONFIG_CACHEPART

/*
 * Garbage Collection Options
 */
#undef NAUT_CONFIG_GARBAGE_COLLECTION

/*
 * FPU Options
 */
#undef NAUT_CONFIG_XSAVE_SUPPORT

/*
 * Optimizations
 */
#define NAUT_CONFIG_FPU_SAVE 1
#undef NAUT_CONFIG_KICK_SCHEDULE
#undef NAUT_CONFIG_HALT_WHILE_IDLE
#undef NAUT_CONFIG_THREAD_OPTIMIZE

/*
 * Debugging
 */
#undef NAUT_CONFIG_DEBUG_INFO
#undef NAUT_CONFIG_DEBUG_PRINTS
#undef NAUT_CONFIG_ENABLE_ASSERTS
#undef NAUT_CONFIG_PROFILE
#undef NAUT_CONFIG_SILENCE_UNDEF_ERR
#undef NAUT_CONFIG_ENABLE_STACK_CHECK
#undef NAUT_CONFIG_ENABLE_REMOTE_DEBUGGING
#undef NAUT_CONFIG_ENABLE_MONITOR
#undef NAUT_CONFIG_DEBUG_VIRTUAL_CONSOLE
#undef NAUT_CONFIG_DEBUG_DEV

/*
 * Address Spaces
 */
#undef NAUT_CONFIG_ASPACES

/*
 * Runtimes
 */
#undef NAUT_CONFIG_LEGION_RT
#undef NAUT_CONFIG_NDPC_RT
#undef NAUT_CONFIG_NESL_RT
#undef NAUT_CONFIG_OPENMP_RT
#undef NAUT_CONFIG_RACKET_RT
#define NAUT_CONFIG_JS_RT 1

/*
 * Devices
 */

/*
 * Serial Options
 */
#undef NAUT_CONFIG_SERIAL_REDIRECT
#undef NAUT_CONFIG_APIC_FORCE_XAPIC_MODE
#undef NAUT_CONFIG_APIC_TIMER_CALIBRATE_INDEPENDENTLY
#undef NAUT_CONFIG_DISABLE_PS2_MOUSE
#undef NAUT_CONFIG_GPIO
#undef NAUT_CONFIG_HPET
#undef NAUT_CONFIG_VIRTIO_PCI
#undef NAUT_CONFIG_E1000_PCI
#undef NAUT_CONFIG_E1000E_PCI
#undef NAUT_CONFIG_MLX3_PCI
#undef NAUT_CONFIG_RAMDISK
#undef NAUT_CONFIG_ATA

/*
 * Filesystems
 */
#undef NAUT_CONFIG_EXT2_FILESYSTEM_DRIVER
#undef NAUT_CONFIG_FAT32_FILESYSTEM_DRIVER

/*
 * Networking
 */
#undef NAUT_CONFIG_NET_ETHERNET
#undef NAUT_CONFIG_NET_COLLECTIVE

/*
 * Languages
 */
#undef NAUT_CONFIG_LOAD_LUA
