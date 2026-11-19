# LittleFS for Bouffalo SDK

`CONFIG_LITTLEFS=y` builds the LittleFS library and flash port. It does not
mount a partition or reserve the PSM partition. Applications that choose their
own partition, mount point, SD card, or other media continue to call the normal
filesystem APIs with those parameters.

`CONFIG_LITTLEFS_KV_EASYFLASH=y` additionally enables the common key-value store.
It selects the partition-table and MTD components, and initializes the store
through component SYSINIT at `SI_SUB_SERVICES` / `SI_ORDER_SECOND`, after MTD startup.
The PSM store is ready before `main()`; initialization failure calls `abort()`.
Applications using this configuration can call `lfs_kv_get_blob()`,
`lfs_kv_set_blob()`, and the other KV APIs directly, without an initialization
call or an application SYSINIT registration.

The store retains the existing PSM partition, `/_ef4_kvs_` namespace, flash
geometry, and mount/format behavior. Enabling the library alone never opts in
to that store. Avoid mounting the same PSM partition independently while the
KV store owns it.

`lfs_kv_init()` and the legacy `easyflash_init()` wrapper remain compatible.
After successful initialization, subsequent calls return `LFS_KV_OK` without
mounting again or allocating another KV mutex. If automatic KV startup is not
selected, callers that initialize the store manually must serialize the first
initialization before using KV APIs.
