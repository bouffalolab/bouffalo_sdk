/**
 * @file fbq_core.h
 * @brief Reference-counted zero-copy Frame Buffer Queue public API.
 *
 * @details
 * FBQ transports pointers to persistent frame descriptors instead of copying
 * descriptor or payload data. A producer owns one reference after allocation,
 * each successful push creates one queue reference, pop transfers that queue
 * reference to a consumer, and fbq_free() releases one reference. The element
 * owner performs the final release when the framework reference count reaches
 * zero.
 *
 * The data-path APIs support both task and interrupt context. In interrupt
 * context, timeout values are ignored and the corresponding FreeRTOS FromISR
 * operations are used.
 */

#ifndef SOLUTION_FBQ_CORE_H
#define SOLUTION_FBQ_CORE_H

#include <stddef.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <queue.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Halt execution when an FBQ invariant is violated. */
#define FBQ_ASSERT(condition)           \
    do {                                \
        if (!(condition)) {             \
            __asm__ volatile("ebreak"); \
            while (1) {                 \
            }                           \
        }                               \
    } while (0)

/** Maximum number of output subscriptions owned by one FBQ controller. */
#define FBQ_OUTPUT_MAX      (5U)
/** Special output ID requesting automatic allocation of a free output slot. */
#define FBQ_OUTPUT_AUTO     UINT16_MAX
/** Element ID used when an external element has no pool-local identifier. */
#define FBQ_ELEM_ID_INVALID UINT16_MAX
/** Convert an output ID into its output-mask bit. */
#define FBQ_OUTPUT_BIT(id)  (1UL << (id))
/** Mask selecting every output slot supported by an FBQ controller. */
#define FBQ_OUTPUT_ALL      ((1UL << FBQ_OUTPUT_MAX) - 1UL)
/** Convert a frame type ID into its type-mask bit. */
#define FBQ_TYPE_BIT(id)     (1UL << (id))
/** Type mask matching every frame type. */
#define FBQ_TYPE_MASK_ALL    UINT32_MAX

/** Round @p value up to the specified power-of-two @p alignment. */
#define FBQ_ALIGN_UP(value, alignment) \
    (((value) + (alignment) - 1U) & ~((alignment) - 1U))
/** Calculate the pointer-aligned stride of an element and its extension. */
#define FBQ_ELEM_STRIDE(ext_size) \
    FBQ_ALIGN_UP(sizeof(fbq_elem_t) + (ext_size), sizeof(uintptr_t))

/** Frame buffer element descriptor. */
typedef struct fbq_elem fbq_elem_t;
/** FBQ controller and element owner. */
typedef struct fbq_ctrl fbq_ctrl_t;
/** Element allocation and final-release operation table. */
typedef struct fbq_elem_ops fbq_elem_ops_t;
/** Queue routing and output-management operation table. */
typedef struct fbq_ops fbq_ops_t;

/**
 * @brief Element cleanup callback invoked before a pooled element is recycled.
 *
 * @param[in,out] elem Element whose framework reference count reached zero.
 * @param[in] user_data User context configured in fbq_config_t.
 *
 * @note The callback may execute in interrupt context and must not block.
 */
typedef void (*fbq_cleanup_fn)(fbq_elem_t *elem, void *user_data);

/**
 * @brief Persistent descriptor for one zero-copy frame buffer.
 *
 * @details
 * One stable descriptor represents one payload throughout its complete FBQ
 * lifetime. The flexible extension area is pointer-aligned and may be cast to
 * a type-specific metadata structure. ext_size is expressed in bytes.
 */
struct fbq_elem {
    fbq_ctrl_t *owner; /**< Controller responsible for final release. */

    void *data;        /**< Payload start address. */
    uint32_t capacity; /**< Maximum payload size in bytes. */
    uint32_t size;     /**< Valid payload size in bytes. */
    uint32_t type_mask; /**< Frame type bits used for subscription filtering. */

    uint16_t id;        /**< Pool-local element ID or FBQ_ELEM_ID_INVALID. */
    uint16_t ref_count; /**< Framework-managed reference count. */

    uint16_t ext_type; /**< Type identifier used to interpret extension data. */
    uint16_t ext_size; /**< Valid extension area size in bytes. */

    uintptr_t extension[]; /**< Pointer-aligned type-specific metadata area. */
};

/**
 * @brief Element allocation and final-release operations.
 *
 * @details
 * alloc is optional for externally supplied elements. free is mandatory for a
 * controller used as an element owner and is called exactly once after the
 * framework reference count reaches zero.
 */
struct fbq_elem_ops {
    /**
     * @brief Acquire an element from an owner-specific source.
     * @param[in] ctrl Element owner controller.
     * @param[out] elem Receives the acquired element pointer.
     * @param[in] timeout Maximum wait time in FreeRTOS ticks.
     * @return FBQ_OK on success; a negative FBQ error otherwise.
     */
    int (*alloc)(fbq_ctrl_t *ctrl, fbq_elem_t **elem, TickType_t timeout);
    /**
     * @brief Return the final framework reference to the native owner.
     * @param[in] ctrl Element owner controller.
     * @param[in,out] elem Element whose framework reference count reached zero.
     * @note The callback may execute in interrupt context and must not block.
     */
    void (*free)(fbq_ctrl_t *ctrl, fbq_elem_t *elem);
};

/**
 * @brief Queue routing and output-management operations.
 *
 * @details
 * Implementations transport fbq_elem_t pointers only. They must not modify the
 * framework reference count; the public data-path API owns that policy. A push
 * implementation must reject an element when its type_mask does not intersect
 * the target output's accept_mask.
 */
struct fbq_ops {
    /** Enqueue one already-referenced element into an output. */
    int (*push)(fbq_ctrl_t *ctrl, fbq_elem_t *elem, uint16_t output_id, TickType_t timeout);
    /** Dequeue an element and transfer the queue reference to the caller. */
    int (*pop)(fbq_ctrl_t *ctrl, fbq_elem_t **elem, uint16_t output_id, TickType_t timeout);

    /** Create and activate an output subscription. */
    int (*output_open)(fbq_ctrl_t *ctrl,
                       uint16_t *output_id,
                       uint16_t depth,
                       uint32_t accept_mask);
    /** Drain and destroy an output subscription. */
    int (*output_close)(fbq_ctrl_t *ctrl, uint16_t output_id);
    /** Return the number of elements waiting in an output subscription. */
    int (*output_count)(fbq_ctrl_t *ctrl, uint16_t output_id);
    /** Return the number of elements available from the owner pool. */
    int (*free_count)(fbq_ctrl_t *ctrl);
};

/** @brief Runtime state of one output subscription. */
typedef struct {
    QueueHandle_t queue;  /**< FreeRTOS queue containing fbq_elem_t pointers. */
    uint32_t accept_mask; /**< Frame type bits accepted by this output. */
    uint16_t depth;       /**< Configured queue depth. */
} fbq_output_t;

/**
 * @brief Frame buffer owner and multi-output queue controller.
 *
 * @details
 * The default fixed-pool implementation uses all fields. An external owner may
 * initialize only elem_ops, name, ext_type, and user_data; unused pool and
 * routing fields remain zero.
 */
struct fbq_ctrl {
    const fbq_elem_ops_t *elem_ops; /**< Element ownership operations. */
    const fbq_ops_t *ops;           /**< Queue routing operations, if any. */

    const char *name;           /**< Human-readable controller name. */
    uint32_t default_type_mask; /**< Type mask restored on each pooled allocation. */
    uint16_t ext_type;          /**< Default extension type for this controller. */
    uint16_t elem_count;        /**< Number of descriptors in the fixed pool. */
    uint16_t elem_stride;       /**< Descriptor stride including extension data. */
    uint16_t ext_size;          /**< Extension area size for pooled elements. */

    void *elem_storage;       /**< Dynamically allocated descriptor storage. */
    size_t elem_storage_size; /**< Allocated descriptor storage size in bytes. */

    void *data_storage;       /**< Optional fixed payload storage base address. */
    size_t data_storage_size; /**< Fixed payload storage size in bytes. */
    uint32_t data_capacity;   /**< Payload capacity of each pooled element. */
    uint32_t data_stride;     /**< Byte stride between adjacent payloads. */

    QueueHandle_t free_queue;             /**< Queue containing available element pointers. */
    fbq_output_t outputs[FBQ_OUTPUT_MAX]; /**< Output subscriptions. */

    fbq_cleanup_fn cleanup; /**< Optional cleanup before pool recycling. */
    void *user_data;        /**< Owner- or application-specific context. */
};

/** @brief Configuration for the default fixed-pool FBQ implementation. */
typedef struct {
    const char *name;           /**< Human-readable controller name. */
    uint32_t default_type_mask; /**< Initial type mask; zero matches no frame type. */
    uint16_t ext_type;          /**< Extension type assigned to every pooled element. */
    uint16_t elem_count;        /**< Number of elements in the fixed pool. */
    uint16_t ext_size;          /**< Per-element extension area size in bytes. */

    uint16_t elem_stride; /**< Descriptor stride, or zero for automatic. */

    void *data_storage;       /**< Optional caller-owned persistent payload storage. */
    size_t data_storage_size; /**< Payload storage size in bytes. */
    uint32_t data_capacity;   /**< Maximum valid bytes in each payload. */
    uint32_t data_stride;     /**< Payload stride, or zero to use data_capacity. */

    fbq_cleanup_fn cleanup; /**< Optional cleanup before pool recycling. */
    void *user_data;        /**< User context passed to cleanup. */
} fbq_config_t;

/** @brief FBQ API result codes. */
enum {
    FBQ_OK = 0,               /**< Operation completed successfully. */
    FBQ_ERR_INVALID = -1,     /**< An argument or configuration is invalid. */
    FBQ_ERR_UNSUPPORTED = -2, /**< The requested operation is not supported. */
    FBQ_ERR_TIMEOUT = -3,     /**< Queue operation timed out or would block. */
    FBQ_ERR_BUSY = -4,        /**< Resource is active or still referenced. */
    FBQ_ERR_NO_RESOURCE = -5, /**< Required queue or output slot is unavailable. */
    FBQ_ERR_FILTERED = -6,    /**< Output filter rejected the element type mask. */
};

/**
 * @brief Initialize a default fixed-pool, multi-output FBQ controller.
 * @param[out] ctrl Controller object to initialize.
 * @param[in] config Fixed-pool configuration and optional payload storage.
 * @retval FBQ_OK Initialization succeeded.
 * @retval FBQ_ERR_INVALID Configuration or storage is invalid.
 * @retval FBQ_ERR_NO_RESOURCE Descriptor allocation or FreeRTOS queue creation failed.
 * @note Descriptor storage is allocated internally as elem_stride * elem_count.
 * @note This function must be called from task context.
 */
int fbq_init(fbq_ctrl_t *ctrl, const fbq_config_t *config);

/**
 * @brief Deinitialize a default FBQ controller.
 * @param[in,out] ctrl Controller to deinitialize.
 * @retval FBQ_OK Controller was deinitialized.
 * @retval FBQ_ERR_INVALID @p ctrl is NULL.
 * @retval FBQ_ERR_BUSY An output remains open or an element is in use.
 * @note Close every output and release every element before calling this API.
 * @note This function must be called from task context.
 */
int fbq_deinit(fbq_ctrl_t *ctrl);

/**
 * @brief Initialize a controller that owns externally supplied elements.
 * @param[out] ctrl Controller object to initialize.
 * @param[in] name Optional human-readable owner name.
 * @param[in] ext_type Default external extension type.
 * @param[in] elem_ops External allocation/final-release operations.
 * @param[in] user_data Owner-specific context.
 * @retval FBQ_OK Owner was initialized.
 * @retval FBQ_ERR_INVALID Required arguments or final free are missing.
 * @note The resulting owner has no queue routing operations by default.
 */
int fbq_owner_init(fbq_ctrl_t *ctrl,
                   const char *name,
                   uint16_t ext_type,
                   const fbq_elem_ops_t *elem_ops,
                   void *user_data);

/**
 * @brief Initialize a persistent external element wrapper.
 * @param[out] elem Persistent wrapper storage to initialize.
 * @param[in] owner Controller responsible for final release.
 * @param[in] data External payload start address.
 * @param[in] capacity Maximum external payload size in bytes.
 * @param[in] size Current valid payload size in bytes.
 * @param[in] type_mask Frame type bits used for subscription filtering.
 * @param[in] ext_type Type identifier used to interpret extension data.
 * @param[in] id Element identifier or FBQ_ELEM_ID_INVALID.
 * @param[in] ext_size Valid extension area size in bytes.
 * @note The initialized wrapper starts with one producer reference.
 */
void fbq_elem_init(fbq_elem_t *elem,
                   fbq_ctrl_t *owner,
                   void *data,
                   uint32_t capacity,
                   uint32_t size,
                   uint32_t type_mask,
                   uint16_t ext_type,
                   uint16_t id,
                   uint16_t ext_size);

/**
 * @brief Allocate an element and acquire one producer reference.
 * @param[in] ctrl Element owner/controller used for allocation.
 * @param[out] elem Receives the allocated element pointer.
 * @param[in] timeout Maximum wait time in FreeRTOS ticks; ignored in ISR context.
 * @return FBQ_OK on success; a negative FBQ error otherwise.
 */
int fbq_alloc(fbq_ctrl_t *ctrl, fbq_elem_t **elem, TickType_t timeout);

/**
 * @brief Push an element to one output subscription.
 * @param[in] target Destination controller, or NULL to use elem->owner.
 * @param[in,out] elem Element to publish.
 * @param[in] output_id Destination output ID.
 * @param[in] timeout Maximum wait time in FreeRTOS ticks; ignored in ISR context.
 * @return FBQ_OK on success; a negative FBQ error otherwise.
 * @note A successful push creates one queue reference. A failed push rolls the
 *       reserved reference back before returning.
 * @note Set type_mask before the first push and keep it unchanged while the
 *       element is shared.
 */
int fbq_push(fbq_ctrl_t *target,
             fbq_elem_t *elem,
             uint16_t output_id,
             TickType_t timeout);

/**
 * @brief Push an element independently to each selected output.
 * @param[in] target Destination controller, or NULL to use elem->owner.
 * @param[in,out] elem Element to publish.
 * @param[in] output_mask Bit mask of output IDs to attempt.
 * @param[in] timeout Per-output wait time in ticks; ignored in ISR context.
 * @return Bit mask of outputs that accepted the element.
 * @note This function does not release the producer reference.
 */
uint32_t fbq_push_mask(fbq_ctrl_t *target,
                       fbq_elem_t *elem,
                       uint32_t output_mask,
                       TickType_t timeout);

/**
 * @brief Pop an element and acquire the output queue's existing reference.
 * @param[in] ctrl Source FBQ controller.
 * @param[out] elem Receives the popped element pointer.
 * @param[in] output_id Source output ID.
 * @param[in] timeout Maximum wait time in FreeRTOS ticks; ignored in ISR context.
 * @return FBQ_OK on success; a negative FBQ error otherwise.
 * @note Pop transfers ownership and does not change the reference count.
 */
int fbq_pop(fbq_ctrl_t *ctrl,
            fbq_elem_t **elem,
            uint16_t output_id,
            TickType_t timeout);

/**
 * @brief Release one producer, queue, or consumer reference.
 * @param[in,out] elem Element reference to release.
 * @note When the count reaches zero, elem->owner->elem_ops->free() is called.
 * @note The final owner callback may invalidate @p elem immediately.
 */
void fbq_free(fbq_elem_t *elem);

/**
 * @brief Create and activate an output subscription.
 * @param[in,out] ctrl Destination FBQ controller.
 * @param[in,out] output_id Requested ID, or FBQ_OUTPUT_AUTO; receives the selected ID.
 * @param[in] depth Number of element pointers that the output can hold.
 * @param[in] accept_mask Frame type bits accepted by this output; zero rejects all elements.
 * @return FBQ_OK on success; a negative FBQ error otherwise.
 * @note Output control operations must be externally serialized.
 * @note This function must be called from task context.
 */
int fbq_output_open(fbq_ctrl_t *ctrl,
                    uint16_t *output_id,
                    uint16_t depth,
                    uint32_t accept_mask);

/**
 * @brief Drain and destroy an output subscription.
 * @param[in,out] ctrl FBQ controller owning the output.
 * @param[in] output_id Output subscription to close.
 * @return FBQ_OK on success; a negative FBQ error otherwise.
 * @note Already-popped references remain owned by their consumers.
 * @note The caller must serialize close against push, pop, count queries, and
 *       other output control operations on the same output.
 * @note This function must be called from task context.
 */
int fbq_output_close(fbq_ctrl_t *ctrl, uint16_t output_id);

/**
 * @brief Get the number of elements waiting in an output subscription.
 * @param[in] ctrl FBQ controller owning the output.
 * @param[in] output_id Output subscription to query.
 * @return Nonnegative waiting count on success; a negative error otherwise.
 */
int fbq_output_count(fbq_ctrl_t *ctrl, uint16_t output_id);

/**
 * @brief Get the number of elements currently available for allocation.
 * @param[in] ctrl Element pool controller.
 * @return Nonnegative free-element count on success; a negative error otherwise.
 */
int fbq_free_count(fbq_ctrl_t *ctrl);

/**
 * @brief Get a writable pointer to an element's extension metadata.
 * @param[in,out] elem Element containing the extension area.
 * @return Pointer to the first extension byte.
 */
static inline void *fbq_elem_extension(fbq_elem_t *elem)
{
    return (void *)elem->extension;
}

#ifdef __cplusplus
}
#endif

#endif /* SOLUTION_FBQ_CORE_H */
