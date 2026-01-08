#pragma once
#include <stdint.h>

/**
 * @file event_manager.h
 * @brief Priority-based event manager with OS abstraction and portable ctz.
 *
 * This header provides:
 *  - OS abstraction for critical sections (task/ISR safe)
 *  - Portable Count Trailing Zeros (ctz) wrapper
 *  - Event manager types and APIs
 *
 * For FreeRTOS, define USE_FREERTOS at compile time.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==============================
 * OS abstraction for critical section
 * ============================== */
#if 1
#include "osif.h"

#define EVENT_OS_ENTER_CRITICAL(mask)   mask = osif_lock()
#define EVENT_OS_EXIT_CRITICAL(mask)    osif_unlock(mask)

/** ISR version: creates a local mask variable */
//#define EVENT_OS_ENTER_CRITICAL_FROM_ISR()  uint32_t mask_var = osif_lock()
//#define EVENT_OS_EXIT_CRITICAL_FROM_ISR()   os_unlock(mask_var)

#else
/* Bare-metal fallback: user must provide __disable_irq / __enable_irq or similar */
#ifndef EVENT_OS_ENTER_CRITICAL
extern void __disable_irq(void);
extern void __enable_irq(void);
#define EVENT_OS_ENTER_CRITICAL()    __disable_irq()
#define EVENT_OS_EXIT_CRITICAL()     __enable_irq()
#endif

/* ISR macros are no-op for bare-metal default */
#define EVENT_OS_ENTER_CRITICAL_FROM_ISR(mask_var)  (void)0
#define EVENT_OS_EXIT_CRITICAL_FROM_ISR(mask_var)   (void)0

#endif /* USE_FREERTOS */

/* ==============================
 * Portable Count Trailing Zeros (ctz) wrapper
 * ============================== */
/**
 * @brief Return the index of the least significant set bit.
 * @note Behavior is undefined if x == 0. Caller must ensure x != 0.
 */
static inline int event_ctz32(uint32_t x)
{
#if defined(__GNUC__) || defined(__clang__)
#warning "Using GCC/Clang __builtin_ctz implementation"
    return __builtin_ctz(x);
#elif defined(_MSC_VER)
#warning "Using MSVC _BitScanForward implementation"
    unsigned long idx;
    _BitScanForward(&idx, x); /* returns 0 if x == 0 */
    return (int)idx;
#else
#warning "Using generic fallback (slow) implementation"
    /* Generic fallback (portable but slower) */
    int i = 0;
    while (x && !(x & 1U)) { x >>= 1; i++; }
    return i;
#endif
}

/* ==============================
 * Configuration (adjust via compiler flags or build system)
 * ============================== */
#ifndef MAX_EVENT_NUM
#define MAX_EVENT_NUM        8
#endif

#ifndef MAX_PRIORITY_NUM
#define MAX_PRIORITY_NUM      4   /**< Support 0..7, 0 = highest */
#endif

/** Dispatch mode:
 * 1 = Preemptive: check higher priority after each event
 * 0 = Non-preemptive: process all high->low in rounds until empty
 */
#ifndef EVENT_PREEMPTIVE_MODE
#define EVENT_PREEMPTIVE_MODE 0
#endif

/* ==============================
 * Types & API
 * ============================== */

/** Event callback type */
typedef void (*event_callback_t)(void);

/** Single event entry */
typedef struct
{
    uint8_t priority;           /**< Event priority (0 = highest) */
    event_callback_t callback;  /**< Callback function for this event */
} event_entry_t;

/** Event manager structure */
typedef struct
{
    uint32_t pending_bits;                         /**< Global pending status (bit = event_id) */
    uint32_t priority_bitmap[MAX_PRIORITY_NUM];    /**< Bitmask of events per priority */
    uint8_t  active_prio_mask;                     /**< Bitmask of non-empty priorities */
    uint8_t  priorities[MAX_EVENT_NUM];           /**< Event priority array */
    event_callback_t callbacks[MAX_EVENT_NUM];    /**< Event callback array */
    uint8_t  event_count;                          /**< Number of registered events */
} event_manager_t;

/* ==============================
 * API
 * ============================== */

/**
 * @brief Initialize an event manager instance.
 * @param mgr Pointer to the event_manager_t instance
 */
void event_manager_init(event_manager_t *mgr);

/**
 * @brief Register a new event with priority and callback.
 * @param mgr Pointer to the event manager
 * @param priority Event priority (0 = highest)
 * @param cb Callback function
 * @return Event ID (>=0) if successful, -1 on error
 */
int  event_register(event_manager_t *mgr, uint8_t priority, event_callback_t cb);

/**
 * @brief Post (set) an event as pending.
 *
 * This function marks the specified event as pending, indicating that
 * its associated callback should be executed during the next dispatch cycle.
 *
 * If the event is already pending, this function has no additional effect;
 * the callback will be executed only once per dispatch cycle.
 *
 * @note This function is safe to call from normal task or thread context.
 *       It does not block and can be invoked repeatedly without accumulating
 *       multiple executions of the same event.
 *
 * @param[in,out] mgr      Pointer to the event manager instance.
 * @param[in]     event_id Event ID returned by ::event_register().
 *
 * @retval None
 */
void event_set(event_manager_t *mgr, int event_id);

/**
 * @brief Dispatch events.
 * @details Call in a single thread/task context to process pending events
 *          according to configured priority and dispatch mode.
 * @param mgr Pointer to the event manager
 */
void event_dispatch(event_manager_t *mgr);

#ifdef __cplusplus
}
#endif
