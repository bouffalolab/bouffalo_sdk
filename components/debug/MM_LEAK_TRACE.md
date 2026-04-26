# MM Leak Trace

## 目的

`mm_leak_trace` 是一个仅用于 debug 的 malloc/free 跟踪功能。它通过 backtrace
聚合分配调用点，统计每个调用点的：

- `malloc_count`
- `free_count`
- `leak = malloc_count - free_count`

实现文件：

- `components/debug/src/mm_leak_trace.c`
- `components/debug/include/mm_leak_trace.h`

接入点：

- `components/mm/mm.c`

## 启用方式

需要同时打开：

- `CONFIG_BACKTRACE`
- `CONFIG_MM_ENABLE_TRACKING`

运行时通过 shell 命令启动：

```text
mm_track start [callsite_cap] [data_cap] [depth] [skip]
```

没有执行 `start` 前，不分配三张主表和 `bt_refcnt` 表。

## 运行时数据结构

### callsite 表

每项记录一个统计调用点：

- `hash`
- `malloc_count`
- `free_count`
- `bt_idx`
- `used`

`callsite` 只按调用栈区分，不再按 size 区分。

### bt_entry 表

每项记录一个唯一 backtrace：

- `hash`
- `data_idx`
- `depth`
- `used`

多个 callsite 可以引用同一个 `bt_entry`。

### bt_refcnt 表

新增的一张运行时数组：

- 类型：`uint16_t[]`
- 大小：`callsite_cap`
- 含义：记录每个 `bt_entry` 当前被多少个 `callsite` 引用

它的作用是把 `clear` 的热点从近似 `O(N^2)` 降到近似 `O(N)`。

### bt_data 表

连续的 `uint32_t` 数组，保存实际 backtrace 地址序列。

`bt_entry.data_idx` 指向这张表中的起始位置。

## malloc/free 工作流

### malloc

1. `mm.c` 申请 `user_size + 4`
2. `mm_leak_trace_malloc()` 抓 backtrace
3. 在 `bt_entry` 中查找或新建唯一 backtrace
4. 在 `callsite` 中查找或新建调用点条目
5. `malloc_count++`
6. 在块尾 4 字节写入 `callsite tag`

### free

1. 从块尾读出 `callsite tag`
2. 立即把块尾 4 字节清零
3. 根据 `tag` 找到对应 callsite 槽位
4. `free_count++`

块尾清零可以帮助发现 double free。第二次 free 往往会读到 `0x00000000`。

## 二级槽与碰撞处理

`hash` 相同但 `bt_idx` 不同，说明不同 backtrace 发生了 hash collision。

当前做法不是报错合并，而是：

- 直接在 `callsite` 表里再占一个空槽
- 同一个 `hash` 可以对应多个不同 `callsite` 槽位
- 这个额外槽位就是“二级槽”

块尾存的是 `callsite tag`，不是裸 hash：

- 高 16 位：由 `hash` 派生出的指纹
- 低 16 位：`callsite` 槽位号

这样 `free` 时可以直接按槽位找到正确的 callsite，而不会把碰撞项混在一起。

## clear 的设计

### clear 前

早期实现中，判断一个 `bt_entry` 是否仍被引用，做法是：

- 对每个 `bt_entry`
- 扫描整个 `callsite` 表

热点复杂度接近：

```text
O(callsite_cap * bt_entry_cap)
```

### clear 后

现在改为使用 `bt_refcnt`：

- 新建 callsite 时：`bt_refcnt[bt_idx]++`
- 删除 balanced callsite 时：`bt_refcnt[bt_idx]--`
- `bt_refcnt[bt_idx] == 0` 时，该 `bt_entry` 可回收

这样 clear 的主要复杂度变为：

```text
O(callsite_cap + bt_entry_cap + bt_data_used)
```

### clear 的执行顺序

必须保持下面顺序，后续维护不要打乱：

1. 删除 `malloc_count == free_count` 的 callsite
2. 对应 `bt_refcnt--`
3. 删除 `bt_refcnt == 0` 的 bt_entry
4. 压缩 `bt_data`
5. 更新保留项的 `data_idx`

## 维护约束

### 1. callsite 生命周期必须维护 bt_refcnt

任何未来修改，只要新增或删除 callsite，都必须同步维护：

- 新建 callsite：`bt_refcnt++`
- 删除 callsite：`bt_refcnt--`

否则 clear 会出现：

- bt_entry 误删
- bt_entry 无法回收

### 2. mm.c 只保留 HOOK 宏

`components/mm/mm.c` 中和 leak trace 相关的逻辑已经收口到文件顶部 HOOK 宏。

后续不要把 debug 条件编译重新散落到 `kmalloc/kfree/krealloc/kmalloc_size`
主逻辑里。

### 3. tag 编码改动要成对修改

如果将来修改 `callsite tag` 格式，必须同时修改：

- malloc 写尾部
- free 解码与校验

## 命令

```text
mm_track start [callsite_cap] [data_cap] [depth] [skip]
mm_track stop
mm_track
mm_track dump
mm_track usage
mm_track clear
mm_track reset
mm_track bt <hash>
mm_track test <1|2|3|4|5|6|7|all>
mm_track perf [rounds] [size] [seed_entries] [seed_depth]
mm_track alloc <size>
mm_track leak <size>
mm_track burst <n> <size>
```

## 建议验证项

### 基本统计

```text
mm_track start
mm_track alloc 64
mm_track leak 128
mm_track dump
```

确认：

- 64 的调用点 `malloc == free`
- 128 的调用点 `malloc > free`

### clear

```text
mm_track clear
mm_track dump
mm_track usage
```

确认：

- balanced 项被清掉
- leak 项保留
- `data_used` 被压缩回收

### 长时间运行

观察：

- `dropped`
- `callsite used`
- `bt_entry used`
- `bt_data used`

必要时调大：

```text
mm_track start <callsite_cap> <data_cap> <depth> <skip>
```
