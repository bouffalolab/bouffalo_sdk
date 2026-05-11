# FreeBSD SYSINIT 系统架构分析

> **目标**: 理解 FreeBSD SYSINIT 的完整设计和实现，提取可移植到嵌入式 MCU 系统的核心思想。

---

## 目录

1. [概述与设计目标](#1-概述与设计目标)
2. [三层架构全景](#2-三层架构全景)
3. [编译期：Linker Set 机制](#3-编译期linker-set-机制)
4. [注册层：SYSINIT 宏展开](#4-注册层sysinit-宏展开)
5. [排序与调度层](#5-排序与调度层)
   - [5.1 内核启动路径：`mi_startup()`](#51-内核启动路径mi_startup)
   - [5.2 模块加载路径：`linker_file_sysinit()`](#52-模块加载路径linker_file_sysinit)
   - [5.3 运行时合并：`sysinit_add()`](#53-运行时合并sysinit_add)
   - [5.4 卸载路径：`linker_file_sysuninit()`](#54-卸载路径linker_file_sysuninit)
6. [全量子系统枚举（SI_SUB_*）](#6-全量子系统枚举si_sub_)
7. [次键排序（SI_ORDER_*）](#7-次键排序si_order_)
8. [高级模式](#8-高级模式)
   - [8.1 DECLARE_MODULE — 内核模块注册](#81-declare_module--内核模块注册)
   - [8.2 VNET_SYSINIT — 网络命名空间复用](#82-vnet_sysinit--网络命名空间复用)
   - [8.3 MTX_SYSINIT / lock 专用变体](#83-mtx_sysinit--lock-专用变体)
   - [8.4 TUNABLE_* — 可配置常量](#84-tunable_--可配置常量)
   - [8.5 TSLOG 时间戳包装](#85-tslog-时间戳包装)
   - [8.6 VERBOSE_SYSINIT 调试支持](#86-verbose_sysinit-调试支持)
   - [8.7 DDB 内核调试器集成](#87-ddb-内核调试器集成)
9. [嵌入式 MCU 适配设计](#9-嵌入式-mcu-适配设计)
   - [9.1 约束分析](#91-约束分析)
   - [9.2 方案 A：编译期静态数组（首选）](#92-方案-a编译期静态数组首选)
   - [9.3 方案 B：ELF Section + 后处理工具](#93-方案-belf-section--后处理工具)
   - [9.4 方案 C：手动注册表（兼容性最佳）](#94-方案-c手动注册表兼容性最佳)
   - [9.5 适配实施指南](#95-适配实施指南)
10. [总结：核心设计原则](#10-总结核心设计原则)

---

## 1. 概述与设计目标

SYSINIT 是 FreeBSD 内核中一个**声明式、可排序、去中心化**的子系统初始化框架。每个需要初始化的子系统只需声明自己的初始化函数和排序信息，无需修改任何集中调度代码。框架在启动时自动收集所有注册项，按其声明的顺序依次执行。

### 核心理念

| 特性 | 说明 |
|------|------|
| **声明式注册** | 使用宏 `SYSINIT(name, sub, order, func, data)` 在任意 `.c` 文件中声明，无需修改头文件或调度器 |
| **两级排序** | `subsystem`（主键，大阶段） + `order`（次键，阶段内顺序），均升序执行 |
| **去中心化** | 各个子系统自描述初始化依赖，调度器本身不感知任何子系统语义 |
| **编译期收集** | 利用 linker set（ELF section）在编译/链接时自动聚合全局注册项 |
| **运行时动态合并** | 内核模块（`.ko`）加载时，其 SYSINIT 集合被合并到已排序的运行链表中 |
| **有序卸载** | `SYSUNINIT` 提供反向 tear-down，确保依赖顺序的逆序释放 |

### 生命周期全景

```
编译期                        链接期                        运行期
  |                             |                             |
  SYSINIT(...)                  __start_set_sysinit_set      mi_startup()
  C_SYSINIT(...)     -------->  __stop_set_sysinit_set        |
  MTX_SYSINIT(...)   linker      |                     [构建有序链表]
  DECLARE_MODULE()   合并进      |                     [STAILQ_MERGESORT]
  TUNABLE_INT(...)   set_段      |   ELF 镜像                [遍历执行 func(udata)]
                                                    |
                                            [动态模块加载: linker_file_sysinit()]
                                            [动态模块卸载: linker_file_sysuninit()]
```

---

## 2. 三层架构全景

```
┌────────────────────────────────────────────────────────────────────┐
│                      第 1 层：编译期注册                           │
│                                                                     │
│  SYSINIT(foo, SI_SUB_DRIVERS, SI_ORDER_MIDDLE, foo_init, &cfg)     │
│      │                                                              │
│      ▼                                                              │
│  static struct sysinit foo_sys_init = {                             │
│      .subsystem = SI_SUB_DRIVERS,  (enum sysinit_sub_id)           │
│      .order     = SI_ORDER_MIDDLE, (enum sysinit_elem_order)       │
│      .next      = { NULL },        (STAILQ_ENTRY)                  │
│      .func      = foo_init,        (sysinit_cfunc_t)               │
│      .udata     = &cfg             (const void *)                  │
│  };                                                                 │
│  __attribute__((section("set_sysinit_set"))) ← linker set 指针     │
│  __used = &foo_sys_init                                             │
└──────────────────────┬──────────────────────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────────────────────┐
│                      第 2 层：ELF Section 聚合                      │
│                                                                     │
│  ELF 镜像布局:                                                      │
│  ┌──────────────────────────────────────────┐                       │
│  │         set_sysinit_set section          │                       │
│  │  __start_set_sysinit_set                 │                       │
│  │  ┌──────────────────────────────────┐    │                       │
│  │  │ &a_sys_init   (SI_SUB_VM, 0x00)  │    │                       │
│  │  │ &b_sys_init   (SI_SUB_CPU, 0x01) │    │  ← 未排序的指针数组  │
│  │  │ &c_sys_init   (SI_SUB_VM, 0x02)  │    │                       │
│  │  │ ...                               │    │                       │
│  │  └──────────────────────────────────┘    │                       │
│  │  __stop_set_sysinit_set                  │                       │
│  └──────────────────────────────────────────┘                       │
│                                                                     │
│  SET_DECLARE(sysinit_set, struct sysinit);  ← 声明边界符号          │
│  SET_BEGIN(sysinit_set)  → &__start_set_sysinit_set                │
│  SET_LIMIT(sysinit_set)  → &__stop_set_sysinit_set                 │
└──────────────────────┬──────────────────────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────────────────────┐
│                      第 3 层：运行期调度                            │
│                                                                     │
│  mi_startup() / linker_file_sysinit()                               │
│      │                                                              │
│      ▼                                                              │
│  1. 遍历 linker set → 构建 STAILQ（单链表）                         │
│  2. STAILQ_MERGESORT (稳定归并排序)                                  │
│  3. while (链表非空) {                                              │
│         sip = STAILQ_FIRST(head);                                   │
│         if (sip->subsystem != SI_SUB_DUMMY)                         │
│             sip->func(sip->udata);  ← 执行                          │
│         STAILQ_REMOVE_HEAD(head);                                   │
│         STAILQ_INSERT_TAIL(done_list, sip);  ← 归档到完成列表       │
│     }                                                                │
│                                                                     │
│  排序规则:                                                          │
│    A < B ⇔ (A.subsystem < B.subsystem) ∨                          │
│             (A.subsystem == B.subsystem ∧ A.order < B.order)       │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 3. 编译期：Linker Set 机制

> **文件**: `sys/sys/linker_set.h`

Linker Set 是 SYSINIT 的基石。它利用 GCC/Clang 的 `__attribute__((section("name")))` 扩展，将不同编译单元中的指针变量放入 ELF 文件的同一个命名 section 中。链接器自动将同 section 的所有项连续排列，并在 section 的首尾各放置一个标记符号。

### 核心宏

```c
// 私有中间宏：生成 section 变量
#define __MAKE_SET_QV(set, sym, qv)                     \
    __WEAK(__CONCAT(__start_set_,set));                 \
    __WEAK(__CONCAT(__stop_set_,set));                  \
    static void const * qv                              \
    __NOASAN                                            \
    __set_##set##_sym_##sym __section("set_" #set)      \
    __used = &(sym)

// 公共宏
#define DATA_SET(set, sym)     __MAKE_SET(set, sym)     // const 指针
#define DATA_WSET(set, sym)    __MAKE_SET_QV(set, sym, ) // 可写指针
#define TEXT_SET(set, sym)     __MAKE_SET(set, sym)     // 代码段
#define BSS_SET(set, sym)      __MAKE_SET(set, sym)     // BSS 段

// 遍历集合
#define SET_DECLARE(set, ptype)         extern ptype *__start_set_##set; extern ptype *__stop_set_##set
#define SET_BEGIN(set)                 (&__CONCAT(__start_set_,set))
#define SET_LIMIT(set)                 (&__CONCAT(__stop_set_,set))
#define SET_FOREACH(pvar, set)         for (pvar = SET_BEGIN(set); pvar < SET_LIMIT(set); pvar++)
#define SET_COUNT(set)                 (SET_LIMIT(set) - SET_BEGIN(set))
```

### 关键点

- **`DATA_WSET` vs `DATA_SET`**: SYSINIT 使用 `DATA_WSET`（非 const），因为 `struct sysinit` 中的 `STAILQ_ENTRY(next)` 在运行时会被修改（链表指针重写）。
- **`__used`**: 防止链接器优化掉"未使用"的变量。
- **`__section("set_" #set)`**: 变量被放入类似 `.rodata.set_sysinit_set` 的 section。
- **Sentinel 符号**: `__start_set_X` 和 `__stop_set_X` 由链接器自动生成（`__WEAK` 声明），标记集合的起止边界。

### ELF Section 生成过程

```
编译期 (per .c file):                          链接后 (kernel ELF):
                                                  ┌────────────────────────────┐
  foo.c:                                          │ set_sysinit_set (rodata)   │
    SYSINIT(foo, SI_SUB_VM, ...)                  │                            │
    → static const void * __set_...               │ __start_set_sysinit_set ──►│ &foo_sys_init  (file a)
      __section("set_sysinit_set")                 │                            │ &bar_sys_init  (file b)
      __used = &foo_sys_init;                     │                            │ &baz_sys_init  (file c)
                                                   │                            │ ...
  bar.c:                                           │ __stop_set_sysinit_set ───│ (end)
    SYSINIT(bar, SI_SUB_CPU, ...)                  └────────────────────────────┘
    → 另一个变量，同一个 section

  baz.c:
    SYSINIT(baz, SI_SUB_VM, ...)
    → 又一个变量，同一个 section
```

> **MCU 适配注意**: 并非所有嵌入式工具链都有成熟的 section 放置机制。ARM GCC/Clang 通常支持 `__attribute__((section(...)))`，但需要正确的 linker script 配置来保留这些 section 并生成边界符号。

---

## 4. 注册层：SYSINIT 宏展开

> **文件**: `sys/sys/kernel.h`

### 核心数据结构

```c
typedef void (*sysinit_nfunc_t)(void *);
typedef void (*sysinit_cfunc_t)(const void *);

struct sysinit {
    enum sysinit_sub_id     subsystem;  // 子系统标识 (主排序键)
    enum sysinit_elem_order order;      // 子系统内顺序 (次排序键)
    STAILQ_ENTRY(sysinit)   next;       // 单链表节点 (运行期使用)
    sysinit_cfunc_t         func;       // 要调用的函数
    const void             *udata;      // 传递给函数的参数
};
```

### C_SYSINIT 宏展开

```c
#define C_SYSINIT(uniquifier, subsystem, order, func, ident)   \
    static struct sysinit uniquifier ## _sys_init = {          \
        subsystem,          /* .subsystem */                   \
        order,              /* .order */                       \
        { NULL },           /* .next — STAILQ_ENTRY 初始化 */  \
        func,               /* .func */                        \
        (ident)             /* .udata */                       \
    };                                                         \
    DATA_WSET(sysinit_set, uniquifier ## _sys_init)
```

展开示例:

```c
// 源码:
C_SYSINIT(announce, SI_SUB_COPYRIGHT, SI_ORDER_FIRST, print_caddr_t, copyright);

// 展开后:
static struct sysinit announce_sys_init = {
    SI_SUB_COPYRIGHT,
    SI_ORDER_FIRST,
    { NULL },
    print_caddr_t,
    copyright
};
// 加上 linker set 放置:
// static const void * __set_sysinit_set_sym_announce_sys_init
//     __section("set_sysinit_set") __used = &announce_sys_init;
```

### SYSINIT vs C_SYSINIT 的区别

| 宏 | func 签名 | udata 类型 | 用途 |
|----|----------|-----------|------|
| `C_SYSINIT` | `void (*)(const void *)` | `const void *` | 参数为常量数据的场景 |
| `SYSINIT` | `void (*)(void *)` | `void *` | 参数为可变数据的场景（通过强制转换消除 `-Wcast-qual` 警告） |

```c
// SYSINIT 对 C_SYSINIT 的包装:
#define SYSINIT(uniquifier, subsystem, order, func, ident)     \
    C_SYSINIT(uniquifier, subsystem, order,                    \
        (sysinit_cfunc_t)(sysinit_nfunc_t)func, (void *)(ident))
```

### SYSUNINIT — 卸载注册

```c
#define C_SYSUNINIT(uniquifier, subsystem, order, func, ident) \
    static struct sysinit uniquifier ## _sys_uninit = {        \
        subsystem, order, { NULL }, func, (ident)              \
    };                                                         \
    DATA_WSET(sysuninit_set, uniquifier ## _sys_uninit)

#define SYSUNINIT(uniquifier, subsystem, order, func, ident)   \
    C_SYSUNINIT(uniquifier, subsystem, order,                  \
        (sysinit_cfunc_t)(sysinit_nfunc_t)func, (void *)(ident))
```

`SYSUNINIT` 使用独立的 linker set `sysuninit_set`，卸载时以**逆序**执行（subsystem 降序，order 降序）。

---

## 5. 排序与调度层

### 5.1 内核启动路径：`mi_startup()`

> **文件**: `sys/kern/init_main.c:258-350`

这是内核初始化总入口（由架构相关的 `machdep.c` 在早期启动完成后调用）。

#### 核心流程

```
mi_startup()
  │
  ├── 1. sysinit_mklist(&sysinit_list, SET_BEGIN(sysinit_set),
  │                     SET_LIMIT(sysinit_set))
  │      ├── 遍历 linker set 中的所有 struct sysinit * 指针
  │      ├── 逐项 STAILQ_INSERT_TAIL 到临时链表
  │      └── STAILQ_MERGESORT 稳定归并排序
  │
  ├── 2. while (sip = STAILQ_FIRST(&sysinit_list)) {
  │        STAILQ_REMOVE_HEAD(&sysinit_list, next)
  │        STAILQ_INSERT_TAIL(&sysinit_done_list, sip, next)
  │        // 这样即使执行过程中有新的 SYSINIT 被合并进来
  │        // （通过 sysinit_add），也不会漏掉已有的项
  │
  │        if (sip->subsystem == SI_SUB_DUMMY) continue
  │
  │        (*(sip->func))(sip->udata)  ← 执行初始化函数
  │
  │        last = sip->subsystem
  │    }
  │
  └── 3. mtx_unlock(&Giant); 永远睡眠
```

#### 排序比较函数

```c
static int
sysinit_compar(struct sysinit *a, struct sysinit *b, void *thunk __unused)
{
    if (a->subsystem < b->subsystem)  return (-1);
    if (a->subsystem > b->subsystem)  return (1);
    if (a->order < b->order)          return (-1);
    if (a->order > b->order)          return (1);
    return (0);  // 相等时顺序未定义
}
```

**关键设计细节**: `mi_startup` 使用 `STAILQ_FIRST` + `STAILQ_REMOVE_HEAD` 而不是 `STAILQ_FOREACH`。这是因为初始化函数内部可能触发模块加载（`kldload`），后者通过 `sysinit_add()` 向运行链表中插入新的 SYSINIT。如果使用 `FOREACH` 遍历，新插入到当前位置之前的项会被跳过。使用 `FIRST` + `REMOVE_HEAD` 保证每次都取当前最早未执行的项。

#### sysinit_mklist 实现

```c
static void
sysinit_mklist(struct sysinitlist *list,
               struct sysinit **set, struct sysinit **set_end)
{
    struct sysinit **sipp;

    STAILQ_INIT(list);
    for (sipp = set; sipp < set_end; sipp++)
        STAILQ_INSERT_TAIL(list, *sipp, next);

    STAILQ_MERGESORT(list, NULL, sysinit_compar, sysinit, next);
}
```

步骤：指针数组 → 单链表 → 归并排序。`STAILQ_MERGESORT` 定义在 `sys/sys/queue_mergesort.h`，是一个模板宏实现的稳定归并排序（O(n log n)），适用于 `STAILQ`、`LIST`、`TAILQ` 三种队列类型。

### 5.2 模块加载路径：`linker_file_sysinit()`

> **文件**: `sys/kern/kern_linker.c:194-247`

当内核模块（`.ko` 文件）被 `kldload` 加载时，此函数处理模块自身的 SYSINIT 集合。

#### 与 `mi_startup` 的关键区别

| 特性 | mi_startup | linker_file_sysinit |
|------|-----------|-------------------|
| 排序算法 | 归并排序 (STAILQ_MERGESORT) | 冒泡排序 (双层 for 循环) |
| 执行容器 | STAILQ 链表 | 直接遍历 linker set 数组 |
| 新项合并 | 通过 sysinit_add 运行时合并 | 原地执行，不合并到全局链表 |
| 锁 | 无（单线程启动） | mtx_lock(&Giant) |

> **为什么模块加载用冒泡？** 模块的 SYSINIT 数量通常远少于内核本体（几十个 vs 数百个）。冒泡排序对小型集合简单直接，归并排序需要构建链表再摧毁，对模块场景反而更重。

```c
static void
linker_file_sysinit(linker_file_t lf)
{
    struct sysinit **start, **stop, **sipp, **xipp, *save;

    // 从模块的 ELF 中查找 "sysinit_set" section
    if (linker_file_lookup_set(lf, "sysinit_set", &start, &stop, NULL) != 0)
        return;

    // 冒泡排序 (子系统升序 → order 升序)
    for (sipp = start; sipp < stop; sipp++) {
        for (xipp = sipp + 1; xipp < stop; xipp++) {
            if ((*sipp)->subsystem < (*xipp)->subsystem ||
                ((*sipp)->subsystem == (*xipp)->subsystem &&
                 (*sipp)->order <= (*xipp)->order))
                continue;
            save = *sipp;
            *sipp = *xipp;
            *xipp = save;
        }
    }

    // 顺序执行
    mtx_lock(&Giant);
    for (sipp = start; sipp < stop; sipp++) {
        if ((*sipp)->subsystem == SI_SUB_DUMMY) continue;
        (*((*sipp)->func))((*sipp)->udata);
    }
    mtx_unlock(&Giant);
}
```

### 5.3 运行时合并：`sysinit_add()`

> **文件**: `sys/kern/init_main.c:211-227`

当内核模块在 `mi_startup` 运行期间被加载时（例如通过 loader 预加载的模块），其 SYSINIT 通过 `sysinit_add()` 合并到正在执行的全局链表中。

```c
void
sysinit_add(struct sysinit **set, struct sysinit **set_end)
{
    struct sysinitlist new_list;

    // 1. 从新集合构建排序链表
    sysinit_mklist(&new_list, set, set_end);

    // 2. 将新链表归并到全局链表中
    STAILQ_MERGE(&sysinit_list, &new_list, NULL,
                 sysinit_compar, sysinit, next);
}
```

`STAILQ_MERGE` 假设两个输入链表均已排序，产生一个排序后的合并链表。这是一个 O(n+m) 的归并操作。

### 5.4 卸载路径：`linker_file_sysuninit()`

> **文件**: `sys/kern/kern_linker.c:250-306`

模块卸载时，以**逆序**执行 `sysuninit_set` 中的 teardown 函数。

```c
static void
linker_file_sysuninit(linker_file_t lf)
{
    // 查找 "sysuninit_set" section
    linker_file_lookup_set(lf, "sysuninit_set", &start, &stop, NULL);

    // 逆序冒泡排序
    for (sipp = start; sipp < stop; sipp++) {
        for (xipp = sipp + 1; xipp < stop; xipp++) {
            if ((*sipp)->subsystem > (*xipp)->subsystem ||
                ((*sipp)->subsystem == (*xipp)->subsystem &&
                 (*sipp)->order >= (*xipp)->order))
                continue;
            save = *sipp; *sipp = *xipp; *xipp = save;
        }
    }

    // 逆序执行
    for (sipp = start; sipp < stop; sipp++)
        (*((*sipp)->func))((*sipp)->udata);
}
```

**逆序逻辑**: `A.subsystem > B.subsystem` → A 排在 B 前面（与 SYSINIT 的 `<` 相反），确保后初始化的先销毁。

---

## 6. 全量子系统枚举（SI_SUB_*）

> **文件**: `sys/sys/kernel.h:95-182`

每个枚举值是一个 28-bit 标识（低 4 位通常为 0，用于 order 子排序）。

```
0x0000000  SI_SUB_DUMMY            不执行，占位/哨兵
0x0700000  SI_SUB_TUNABLES         建立可调参数 (tunables)
0x0800001  SI_SUB_COPYRIGHT        首次使用控制台 (打印版权)
0x1000000  SI_SUB_VM               虚拟机系统初始化
0x1100000  SI_SUB_COUNTER          counter(9) 初始化
0x1800000  SI_SUB_KMEM             内核内存分配器 (malloc)
0x1A40000  SI_SUB_HYPERVISOR       Hypervisor 检测
0x1A80000  SI_SUB_WITNESS          Witness 锁顺序检查器
0x1AC0000  SI_SUB_MTX_POOL_DYNAMIC 动态 mutex 池
0x1B00000  SI_SUB_LOCK             各种 lock 初始化 (MTX_SYSINIT 等)
0x1C00000  SI_SUB_EVENTHANDLER     事件处理器初始化
0x1E00000  SI_SUB_VNET_PRELINK     VNET 模块前初始化
0x2000000  SI_SUB_KLD              KLD / 模块注册
0x2100000  SI_SUB_CPU              CPU 资源初始化
0x2140000  SI_SUB_KDTRACE          DTrace 钩子
0x2160000  SI_SUB_RANDOM           随机数生成器
0x2180000  SI_SUB_MAC              MAC 安全框架
0x21E0000  SI_SUB_VNET             VNET 0 初始化
0x2200000  SI_SUB_INTRINSIC        proc0 初始化
0x2300000  SI_SUB_VM_CONF          VM 配置、设置限制
0x2400000  SI_SUB_RUN_QUEUE        运行队列
0x2500000  SI_SUB_CREATE_INIT      创建 init 进程 (PID 1)
0x2600000  SI_SUB_SCHED_IDLE       idle 进程
0x2700000  SI_SUB_MBUF             mbuf 子系统
0x2800000  SI_SUB_INTR             中断线程
0x2880000  SI_SUB_TASKQ            任务队列
0x2900000  SI_SUB_SMP              AP 启动 (EARLY_AP_STARTUP)
0x2F00000  SI_SUB_DEVFS            devfs 就绪
0x3000000  SI_SUB_INIT_IF          网络接口初始化准备
0x3100000  SI_SUB_DRIVERS          设备驱动初始化 (NEWBUS)
0x3800000  SI_SUB_CONFIGURE        设备探测/配置
0x4000000  SI_SUB_VFS              虚拟文件系统
0x4800000  SI_SUB_CLOCKS           实时钟/统计钟
0x7000000  SI_SUB_PSEUDO           伪设备
0x7400000  SI_SUB_EXEC             execve() 处理器注册
0x8000000  SI_SUB_PROTO_BEGIN      协议栈 VNET 初始化起始
0x8800000  SI_SUB_PROTO_DOMAIN     协议域注册 (AF_INET 等)
0x9000000  SI_SUB_KPROF            内核性能剖析
0xa000000  SI_SUB_KICK_SCHEDULER   超时事件启动
0xb000000  SI_SUB_ROOT_CONF        查找 root 设备
0xd800000  SI_SUB_SYSCALLS         系统调用注册
0xe000000  SI_SUB_KTHREAD_INIT     init 内核线程
0xe400000  SI_SUB_KTHREAD_PAGE     pageout 守护线程
0xe800000  SI_SUB_KTHREAD_VM       VM 守护线程
0xea00000  SI_SUB_KTHREAD_BUF      buffer 守护线程 (bufdaemon)
0xec00000  SI_SUB_KTHREAD_UPDATE   update 守护线程
0xee00000  SI_SUB_KTHREAD_IDLE     idle 线程
0xf000000  SI_SUB_SMP              AP 启动 (非 EARLY 模式)
0xfffffff  SI_SUB_LAST             最终初始化 (调度结束)
```

**子系统的设计智慧**: 值作为 28-bit 十六进制数，为相邻子系统之间留出充足的间隙（`0x1800000` → `0x1A40000` 之间有 `0x240000` 的空间），便于后续插入新的初始化阶段而无需重新编号所有后续项。

---

## 7. 次键排序（SI_ORDER_*）

> **文件**: `sys/sys/kernel.h:187-198`

```c
enum sysinit_elem_order {
    SI_ORDER_FIRST    = 0x0000000,  // 最早
    SI_ORDER_SECOND   = 0x0000001,
    SI_ORDER_THIRD    = 0x0000002,
    SI_ORDER_FOURTH   = 0x0000003,
    SI_ORDER_FIFTH    = 0x0000004,
    SI_ORDER_SIXTH    = 0x0000005,
    SI_ORDER_SEVENTH  = 0x0000006,
    SI_ORDER_EIGHTH   = 0x0000007,
    SI_ORDER_MIDDLE   = 0x1000000,  // 中间
    SI_ORDER_ANY      = 0xfffffff   // 最后
};
```

**使用模式**: 多数初始化只需 `SI_ORDER_FIRST` / `SI_ORDER_MIDDLE` / `SI_ORDER_ANY`，精确排序用 `SECOND`～`EIGHTH`。不关心顺序时用 `ANY`（排在最后，且与其他 `ANY` 的相对顺序未定义）。

---

## 8. 高级模式

### 8.1 DECLARE_MODULE — 内核模块注册

> **文件**: `sys/sys/module.h:142-155`

```c
#define DECLARE_MODULE_WITH_MAXVER(name, data, sub, order, maxver)  \
    MODULE_DEPEND(name, kernel, __FreeBSD_version,                  \
        __FreeBSD_version, maxver);                                 \
    MODULE_METADATA(_md_##name, MDT_MODULE, &data, __XSTRING(name));\
    SYSINIT(name##module, sub, order, module_register_init, &data); \
    struct __hack

#define DECLARE_MODULE(name, data, sub, order)                      \
    DECLARE_MODULE_WITH_MAXVER(name, data, sub, order, MODULE_KERNEL_MAXVER)
```

展开后产生三项：
1. **`MODULE_DEPEND`** — 内核版本依赖检查元数据
2. **`MODULE_METADATA`** — 模块元数据（名称、事件处理函数）
3. **`SYSINIT`** — 在指定 `sub` 阶段调用 `module_register_init(&data)`

`module_register_init()` 负责将模块注册到全局模块链表，并在注册过程中调用模块的 `MOD_LOAD` 事件处理器。

### 8.2 VNET_SYSINIT — 网络命名空间复用

> **文件**: `sys/net/vnet.h:339`

```c
#define VNET_SYSINIT(ident, subsystem, order, func, arg)    \
    SYSINIT(vnet_##ident##_init, SI_SUB_INIT_IF,            \
        SI_ORDER_ANY, vnet_register_sysinit,                \
        &(struct vnet_sysinit){                             \
            .subsystem = subsystem, .order = order,         \
            .func = func, .arg = arg, .name = #ident        \
        })
```

VNET_SYSINIT 是一种**两级委托**：它先通过普通 SYSINIT 在 `SI_SUB_INIT_IF` 阶段注册一个 `vnet_sysinit` 结构体到 VNET 框架，之后每个 VNET（网络命名空间）创建时，VNET 框架再按 subsystem/order 对已注册项排序并执行。

**设计意义**: 这种委托模式使得 SYSINIT 也可以在非启动场景下复用 — 将 SYSINIT 的注册与执行解耦，绑到一个独立的"执行时机"（这里是 VNET 创建事件）。

### 8.3 MTX_SYSINIT / lock 专用变体

锁子系统通过专用宏自动在 `SI_SUB_LOCK` 阶段注册初始化：

```c
// sys/sys/mutex.h
#define MTX_SYSINIT(name, mtx, desc, opts)                  \
    static void name##_mtx_sysinit(void *arg) {             \
        mtx_init((struct mtx *)arg, #name, NULL, opts);     \
    }                                                       \
    SYSINIT(name##_mtx_sysinit, SI_SUB_LOCK, SI_ORDER_MIDDLE,\
        name##_mtx_sysinit, mtx)

// sys/sys/sx.h
#define SX_SYSINIT(name, sx, desc)    /* 类似 */

// sys/sys/rwlock.h
#define RW_SYSINIT(name, rw, desc)    /* 类似 */
```

这些宏使得锁的初始化完全声明式：

```c
struct mtx my_lock;
MTX_SYSINIT(my_lock_init, &my_lock, "My subsystem lock", MTX_DEF);
// 无需在代码中显式调用 mtx_init()，SYSINIT 框架自动处理
```

### 8.4 TUNABLE_* — 可配置常量

> **文件**: `sys/sys/kernel.h:305-466`

TUNABLE 系统同样构建于 SYSINIT 之上，始终在 `SI_SUB_TUNABLES` 阶段执行：

```c
#define TUNABLE_INT(path, var)                              \
    static struct tunable_int __CONCAT(__tunable_int_, __LINE__) = { \
        (path), (var),                                      \
    };                                                      \
    SYSINIT(__CONCAT(__Tunable_init_, __LINE__),            \
        SI_SUB_TUNABLES, SI_ORDER_MIDDLE, tunable_int_init, \
        &__CONCAT(__tunable_int_, __LINE__))
```

应用示例：
```c
static int my_feature_enabled = 0;
TUNABLE_INT("hw.my_feature.enable", &my_feature_enabled);
// 启动时自动从 loader 环境变量读取值
```

### 8.5 TSLOG 时间戳包装

> **文件**: `sys/sys/kernel.h:246-278`

当启用 `TSLOG`（时间戳日志）时，`C_SYSINIT` 宏生成一个额外的 `sysinit_tslog` 结构体，记录函数名用于启动时间追踪：

```c
#ifdef TSLOG
struct sysinit_tslog {
    sysinit_cfunc_t func;
    const void *data;
    const char *name;    // ← 函数名，用于时间戳日志
};

#define C_SYSINIT(uniquifier, subsystem, order, func, ident)     \
    static struct sysinit_tslog uniquifier ## _sys_init_tslog = {\
        func, (ident), #uniquifier                              \
    };                                                           \
    static struct sysinit uniquifier ## _sys_init = {            \
        subsystem, order, { NULL },                              \
        sysinit_tslog_shim,    // ← 包装函数，先记时间再调原函数 \
        &uniquifier ## _sys_init_tslog                           \
    };                                                           \
    DATA_WSET(sysinit_set, uniquifier ## _sys_init)
#endif
```

### 8.6 VERBOSE_SYSINIT 调试支持

> **文件**: `sys/kern/init_main.c:276-331`

编译时启用 `VERBOSE_SYSINIT` 后，`mi_startup` 会打印每个 SYSINIT 的执行信息：

```
subsystem 1000000
   vm_mem_init(&(null))... done.
   uma_startup(&(null))... done.
subsystem 1800000
   malloc_init(&(null))... done.
```

如果同时启用了 DDB，还会解析函数名和参数名的符号。

### 8.7 DDB 内核调试器集成

> **文件**: `sys/kern/init_main.c:879-927`

DDB 提供 `show sysinit` 命令，遍历 `sysinit_done_list` + `sysinit_list` 显示已完成和待执行的 SYSINIT，包含 subsystem/order 值、函数名、参数指针。

---

## 9. 嵌入式 MCU 适配设计

### 9.1 约束分析

| FreeBSD 特性 | 依赖 | MCU 可行性 |
|-------------|------|-----------|
| `__attribute__((section(...)))` | GCC/Clang + ELF linker | ✅ 多数 ARM GCC 支持，需 linker script 配合 |
| `__start_set_X` / `__stop_set_X` 边界符号 | 链接器自动生成 | ⚠️ 取决于 linker script；GNU ld 通常支持 |
| `STAILQ` 单链表 | 运行时堆分配或静态节点 | ⚠️ 堆可能受限或不可用 |
| `STAILQ_MERGESORT` 归并排序 | O(n log n) 比较，栈深度 | ⚠️ ROM/flash 中排序无意义（静态数组应编译时排序） |
| 动态模块加载 (`kldload`) | ELF 解析器，VFS | ❌ 绝大多数 MCU 无此能力 |
| 运行时 `sysinit_add()` | 动态内存 + 链表 | ❌ 无动态模块则不需要 |
| SYSUNINIT 卸载 | 模块卸载 | ❌ 通常不需要（MCU 极少卸载代码） |
| `Giant` mutex | SMP 锁 | ❌ 单核 MCU 无此需求 |

**结论**: 对于 MCU，保留 SYSINIT 的核心价值（声明式注册 + 有序执行），去除动态模块、SYSUNINIT、链表排序等不需要的特性，用静态数组替代链表。

### 9.2 方案 A：编译期静态数组（首选）

**核心思想**: 使用 linker section 聚合，在 `.init_array` 或自定义 section 中构建已排序的初始化表。

#### 架构设计

```
编译期:                          链接期:                        运行期:
                                  ┌───────────────────────────┐
SYSINIT(uart, 10, uart_init)     │  .sysinit.array section    │
  → 放入 .sysinit.array          │                            │
                                  │ __sysinit_array_start ───►│ [0] {prio=1,  early_clock,   &clock_cfg}
SYSINIT(gpio, 20, gpio_init)     │                            │ [1] {prio=10, uart_init,     NULL}
  → 放入 .sysinit.array          │                            │ [2] {prio=20, gpio_init,     NULL}
                                  │                            │ [3] {prio=30, spi_flash_init, &flash_cfg}
SYSINIT(spi_flash, 30, ...)      │ __sysinit_array_stop ──────│ (end)
  → 放入 .sysinit.array          └───────────────────────────┘
```

#### Linker Script 片段

```ld
SECTIONS {
    .sysinit.array : {
        PROVIDE(__sysinit_array_start = .);
        KEEP(*(.sysinit.array))
        PROVIDE(__sysinit_array_stop = .);
    } > FLASH
}
```

#### 头文件实现

```c
// sysinit.h — 嵌入式 MCU 版本

#ifndef _MCU_SYSINIT_H_
#define _MCU_SYSINIT_H_

#include <stdint.h>

/* 初始化项结构体 */
struct sysinit {
    uint8_t     priority;   /* 优先级 (0 = 最早, 255 = 最晚) */
    void      (*init_fn)(const void *arg);
    const void *arg;
    const char *name;       /* 可选：调试用名称 */
};

/* 注册宏 */
#define SYSINIT(_name, _prio, _fn, _arg)                           \
    static const struct sysinit _sysinit_##_name                   \
        __attribute__((__section__(".sysinit.array"), __used__)) = {\
        .priority = (_prio),                                       \
        .init_fn  = (_fn),                                         \
        .arg      = (const void *)(_arg),                          \
        .name     = #_name,                                        \
    }

/* 区间声明 */
extern const struct sysinit __sysinit_array_start[];
extern const struct sysinit __sysinit_array_stop[];

/* 迭代器 */
#define SYSINIT_FOREACH(_p)                                        \
    for (const struct sysinit *_p = __sysinit_array_start;        \
         _p < __sysinit_array_stop; _p++)

/* 条目计数 */
#define SYSINIT_COUNT()                                            \
    ((size_t)(__sysinit_array_stop - __sysinit_array_start))

#endif /* _MCU_SYSINIT_H_ */
```

#### 运行时调度器

```c
// sysinit_run.c

#include "sysinit.h"

/* 插入排序（用于运行时注册——可选） */
static void sysinit_insert(struct sysinit *slot,
                           const struct sysinit *item)
{
    /* 简单场景下可省略 */
}

/* 启动时执行所有 SYSINIT */
void sysinit_run_all(void)
{
    SYSINIT_FOREACH(sip) {
        if (sip->init_fn) {
            sip->init_fn(sip->arg);
        }
    }
}
```

**优点**:
- 零动态内存分配
- 零运行时排序开销（编译时/链接时已定序）
- 代码极其精简（~50 行头文件 + ~20 行调度器）
- 依赖仅 GCC section attribute，几乎所有 MCU 工具链支持

**缺点**:
- Linker 按编译单元链接顺序排列 section 内的项，**不保证**按 priority 升序。需要后处理步骤重新排序。

### 9.3 方案 B：ELF Section + 后处理工具

**解决 linker 不定序问题**: FreeBSD 的 USB 加载器有一个独立工具 `stand/usb/tools/sysinit.c`，作为后处理步骤读取 ELF section 内容、排序、生成 C 数组文件。

#### 工作流

```
1. 编译 → ELF 镜像
2. objcopy 提取 .sysinit.array section → sysinit.bin (二进制 raw data)
3. 后处理工具 (sysinit_sort) 读取 sysinit.bin，排序，输出 C 源码
4. 将生成的 C 源码编译进最终镜像
```

#### Linker Script 调整

```ld
SECTIONS {
    .sysinit.array : {
        PROVIDE(__sysinit_array_start = .);
        KEEP(*(.sysinit.array))
        PROVIDE(__sysinit_array_stop = .);
    } > FLASH
}
```

#### Makefile 片段

```makefile
# 提取 section
sysinit.bin: firmware.elf
	$(OBJCOPY) -O binary -j .sysinit.array $< $@

# 排序后生成 C 数组
sysinit_sorted.c: sysinit.bin sysinit_sort
	./sysinit_sort -i sysinit.bin -o $@ -s sysinit_sorted

# 将排序后的数组链接进最终固件
firmware.elf: ... sysinit_sorted.o
```

#### 后处理工具核心逻辑（Python 示例）

```python
#!/usr/bin/env python3
import struct, sys

SYSINIT_STRUCT_FMT = "Bxx7sPP"  # priority(u8) + pad(3) + init_fn(P) + arg(P)
SYSINIT_SIZE = 16

def main(input_bin, output_c):
    with open(input_bin, "rb") as f:
        data = f.read()

    entries = []
    for i in range(0, len(data), SYSINIT_SIZE):
        chunk = data[i:i+SYSINIT_SIZE]
        prio, fn, arg = struct.unpack_from(SYSINIT_STRUCT_FMT, chunk)
        entries.append((prio, fn, arg))

    entries.sort(key=lambda e: e[0])

    with open(output_c, "w") as f:
        f.write("#include <stdint.h>\n")
        f.write("struct sysinit { uint8_t prio; void (*fn)(const void*); const void *arg; };\n")
        f.write("const struct sysinit sysinit_sorted[] = {\n")
        for prio, fn, arg in entries:
            f.write(f"    {{ {prio}, (void(*)(const void*))0x{fn:x}, (const void*)0x{arg:x} }},\n")
        f.write("};\n")
        f.write(f"const int sysinit_count = {len(entries)};\n")
```

此方案是 FreeBSD `stand/usb/tools/sysinit.c` 模式的直接仿制。

### 9.4 方案 C：手动注册表（兼容性最佳）

当工具链不支持 section 放置时，使用全局数组手动注册。

```c
// sysinit_manual.h

struct sysinit {
    uint8_t    priority;
    void     (*init_fn)(void);
    const char *name;
};

/* 所有子系统在此声明 */
extern void uart_init(void);
extern void gpio_init(void);
extern void spi_flash_init(void);

/* 集中注册表 */
static const struct sysinit sysinit_table[] = {
    { 1,  uart_init,      "uart"       },
    { 10, gpio_init,      "gpio"       },
    { 20, spi_flash_init, "spi_flash"  },
};

/* 运行时 */
static void sysinit_run(const struct sysinit *table, size_t count)
{
    /* 编译期已知排序，直接遍历 */
    for (size_t i = 0; i < count; i++) {
        if (table[i].init_fn) {
            table[i].init_fn();
        }
    }
}

static inline void sysinit_start(void)
{
    sysinit_run(sysinit_table,
        sizeof(sysinit_table) / sizeof(sysinit_table[0]));
}
```

**优点**: 零工具链依赖，适用于任何 C 编译器。
**缺点**: 失去去中心化注册的便利性（所有模块需写入同一个文件/数组）。

### 9.5 适配实施指南

| 考虑因素 | 推荐方案 |
|---------|---------|
| **工具链**: GCC/Clang + GNU ld, section 可用 | 方案 A（纯静态 section）+ C 预处理排序 |
| **工具链**: GCC/Clang + GNU ld, 需要精确排序 | 方案 B（后处理工具排序） |
| **工具链**: IAR / Keil / 自定义链接器 | 方案 C（手动注册表） |
| **需要运行时注册** (例如 bootloader 传入) | 方案 A + 保留数组末尾空白槽 + 插入排序 |
| **裸机 Bare-metal** (无堆) | 方案 A 或 C |
| **RTOS (FreeRTOS/Zephyr 等)** | 方案 A，与 RTOS init 钩子集成 |

#### 两级排序的简化

FreeBSD 的 subsystem（28-bit） + order（28-bit）两级排序对 MCU 过度设计。简化方案：

```c
// 单级 8-bit 优先级 (0=最早, 255=最晚)
// 时间精度足够大多数 MCU 场景 (256 级)
struct sysinit {
    uint8_t  priority;    // 0-255
    void   (*init_fn)(void);
};

// 如果确实需要两级排序，使用 16-bit 主键 + 8-bit 次键:
struct sysinit_2level {
    uint16_t subsystem;   // 主键
    uint8_t  order;       // 次键
    void   (*init_fn)(void);
};
```

#### 与 RTOS 的集成模式

```c
// FreeRTOS 风格：在调度器启动前执行
int main(void) {
    hardware_init();         // CPU 时钟、看门狗
    sysinit_run_phase(0x00); // 0x00-0x4F: 基础外设 (UART, GPIO)
    sysinit_run_phase(0x80); // 0x50-0xAF: 中间件 (文件系统、网络栈)
    rtos_scheduler_start();  // 启动调度器
    // 不再返回
}

// Zephyr 风格: 直接复用其 SYS_INIT 宏
// 两者理念几乎相同 (已内置在 Zephyr 中)
```

---

## 10. 总结：核心设计原则

从 FreeBSD SYSINIT 提取的**可移植设计原则**：

| # | 原则 | FreeBSD 实现 |
|---|------|-------------|
| 1 | **去中心化注册** | 每个 `.c` 文件自描述其初始化需求，无需修改调度器代码 |
| 2 | **声明式排序** | 枚举值指定执行顺序，编译器/链接器保证正确性 |
| 3 | **编译期收集** | Linker set 在链接时自动聚合所有注册项 |
| 4 | **薄调度层** | `mi_startup` 仅 ~100 行，完全不知晓子系统语义 |
| 5 | **稳定排序保证** | 归并排序 (stable sort)，同 priority 下保持链接顺序 |
| 6 | **可选的动态扩展** | `sysinit_add()` 支持运行时合并新注册项（MCU 可省略） |
| 7 | **调试可观测性** | VERBOSE_SYSINIT + DDB 命令，MCU 可用串口日志替代 |
| 8 | **宏包装隔离** | TUNABLE、MTX_SYSINIT、VNET_SYSINIT 复用同一基础设施 |
| 9 | **不可变注册表** | 注册项是 `static struct`，编译后不可变（防止运行时损坏） |
| 10 | **逆序卸载** | SYSUNINIT 保证依赖的逆序释放 |

**对于嵌入式 MCU 的最简移植**: 保留原则 1-4 和 9，用静态数组 + section 替代链表，单级 priority 替代两级 subsystem+order。核心代码量可控制在 ~100 行 C 头文件 + ~20 行调度循环。
