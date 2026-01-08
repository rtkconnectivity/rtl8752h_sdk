/*
 * =========================================================
 * event_manager.c
 *
 * Implementation of Event Manager module.
 * Provides priority-based event registration, signaling, and dispatching.
 * Uses separate arrays for priority and callback to reduce RAM.
 * =========================================================
 */

#include "event_manager.h"

/* ==================== Internal Helper ==================== */
/* Clear a pending event atomically */
static inline void clear_event_bit(event_manager_t *mgr, int event_id)
{
    uint8_t prio = mgr->priorities[event_id];
    uint32_t mask = (1U << event_id);
    uint32_t irq_mask;

    EVENT_OS_ENTER_CRITICAL(irq_mask);
    mgr->pending_bits         &= ~mask;
    mgr->priority_bitmap[prio] &= ~mask;
    if (mgr->priority_bitmap[prio] == 0U)
    {
        mgr->active_prio_mask &= ~(1U << prio);
    }
    EVENT_OS_EXIT_CRITICAL(irq_mask);
}

/* ==================== Public APIs ==================== */

void event_manager_init(event_manager_t *mgr)
{
    uint32_t irq_mask;

    EVENT_OS_ENTER_CRITICAL(irq_mask);
    mgr->pending_bits     = 0U;
    mgr->active_prio_mask = 0U;
    mgr->event_count      = 0U;
    for (int i = 0; i < MAX_PRIORITY_NUM; ++i)
    {
        mgr->priority_bitmap[i] = 0U;
    }
    EVENT_OS_EXIT_CRITICAL(irq_mask);
}

int event_register(event_manager_t *mgr, uint8_t priority, event_callback_t cb)
{
    uint32_t irq_mask;

    if (!mgr) { return -1; }
    if (priority >= MAX_PRIORITY_NUM) { return -1; }

    EVENT_OS_ENTER_CRITICAL(irq_mask);
    if (mgr->event_count >= MAX_EVENT_NUM)
    {
        EVENT_OS_EXIT_CRITICAL(irq_mask);
        return -1;
    }

    int id = (int)mgr->event_count++;
    mgr->priorities[id] = priority;
    mgr->callbacks[id]  = cb;
    EVENT_OS_EXIT_CRITICAL(irq_mask);

    return id;
}

void event_set(event_manager_t *mgr, int event_id)
{
    if (!mgr) { return; }
    if (event_id < 0) { return; }
    if ((uint32_t)event_id >= mgr->event_count) { return; }

    uint8_t prio = mgr->priorities[event_id];
    uint32_t mask = (1U << event_id);
    uint32_t irq_mask;

    EVENT_OS_ENTER_CRITICAL(irq_mask);
    mgr->pending_bits         |= mask;
    mgr->priority_bitmap[prio] |= mask;
    mgr->active_prio_mask     |= (1U << prio);
    EVENT_OS_EXIT_CRITICAL(irq_mask);
}

/* ==================== Internal Helper ==================== */
static bool event_process_one(event_manager_t *mgr, uint8_t prio)
{
    uint32_t bits;
    uint32_t irq_mask;

    EVENT_OS_ENTER_CRITICAL(irq_mask);
    bits = mgr->priority_bitmap[prio];
    EVENT_OS_EXIT_CRITICAL(irq_mask);

    if (bits == 0U) { return false; }

    int event_id = event_ctz32(bits);

    clear_event_bit(mgr, event_id);

    event_callback_t cb = mgr->callbacks[event_id];
    if (cb) { cb(); }

    return true;
}

/* ==================== Event Dispatcher ==================== */
void event_dispatch(event_manager_t *mgr)
{
    if (!mgr) { return; }

#if EVENT_PREEMPTIVE_MODE
    while (mgr->pending_bits)
    {
        uint32_t active = mgr->active_prio_mask;
        if (active == 0U) { break; }

        uint8_t highest_prio = (uint8_t)event_ctz32(active);

        (void)event_process_one(mgr, highest_prio);
    }
#else
    while (mgr->pending_bits)
    {
        for (uint8_t prio = 0; prio < MAX_PRIORITY_NUM; ++prio)
        {
            while (event_process_one(mgr, prio)) { }
        }
    }
#endif
}
