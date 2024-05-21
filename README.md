# fmem-uart

Uses fmem, either on a device exposed through an [fmem driver](https://github.com/CTSRD-CHERI/freebsd-morello/blob/stratix10/sys/arm64/intel/fmem.c) or a [userspace filesystem](https://github.com/CTSRD-CHERI/cheri-bgas-fuse-devfs), to poll a memory-mapped UART like those exposed in [de10pro-cheri-bgas](https://github.com/CTSRD-CHERI/DE10Pro-cheri-bgas/blob/d82c060ed27b766fb2a01fd1162a1f1998513154/bluespec/CHERI_BGAS_System.bsv#L398).
