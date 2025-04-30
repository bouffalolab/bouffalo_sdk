#ifndef CONFIG_H_RQWZBN7W
#define CONFIG_H_RQWZBN7W

/* Card status check
 *
 * When card death is detected, blctld is notified. User program may reset card.
 *
 * set 0 to disable, otherwise enable
 */
#define CARD_DEAD_CHECK 0

/* Card status check interval and threshold to determine if card is dead
 *
 * Card status is checked every CHECK_CARD_DEAD_INTERVAL_MS.
 * If PING message is not received from card after CARD_DEAD_THRESHOLD_MS, card
 * is considered dead.
 */
#define CHECK_CARD_DEAD_INTERVAL_MS 2000
#define CARD_DEAD_THRESHOLD_MS      3000

/* Number of tty devices to create
 */
#define TTY_DEV_COUNT 1

#endif /* end of include guard: CONFIG_H_RQWZBN7W */
