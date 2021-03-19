#ifndef COMPONENTS_FIBOCOM_OPENCPU_OC_CCID_H
#define COMPONENTS_FIBOCOM_OPENCPU_OC_CCID_H

#include "ccid_message.h"

/**
 * \brief ccid driver
 */
typedef struct drv_ccid drvCCID_t;

/**
 * \brief function type callback on ccid device line state change
 */
typedef void (*ccidLineCallback_t)(void *priv, bool online);

/**
 * \brief open the ccid device
 *
 * \param name  the CCID device name
 * \return
 *      - NULL  fail
 *      - other point of the instance
 */
drvCCID_t *fibo_usb_ccid_open(uint32_t name);

/**
 * \brief set CCID line change callback
 * \note the callback will be called in interrupt
 *
 * \param ccid  the CCID device
 * \param cb    callback on line state change
 * \param ctx   caller context
 */
void fibo_usb_ccid_set_line_change_callback(drvCCID_t *ccid, ccidLineCallback_t cb, void *ctx);

/**
 * \brief get CCID device online or not (connect to a host and enable the config)
 *
 * \param ccid  the CCID device
 * \return
 *      - true if online else false
 */
bool fibo_usb_ccid_online(drvCCID_t *ccid);

/**
 * \brief read CCID slot state, each bit indicated a slot
 *
 * \param ccid      the CCID device
 * \param slotmap   the slotmap must non-null
 * \return
 *      - 0                     succeed
 *      - (negative integer)    negative error number
 */
int fibo_usb_ccid_slot_check(drvCCID_t *ccid, uint8_t *slotmap);

/**
 * \brief slot state change
 *
 * \param ccid      the ccid device
 * \param index     slot index
 * \param insert    insert or remove
 * \return
 *      - 0                     succeed
 *      - (negative integer)    negative error number
 */
int fibo_usb_ccid_slot_change(drvCCID_t *ccid, uint8_t index, bool insert);

/**
 * \brief read CCID device
 * \note this api is synchronous, if no data coming, the call will block
 *
 * \param ccid  the CCID device
 * \param buf   buf to store read data
 * \param size  buffer size, must greater than struct ccid_bulk_out_header
 * \return
 *      - (negative integer)    negative error number
 *      - other                 actual read size
 */
int fibo_usb_ccid_read(drvCCID_t *ccid, struct ccid_bulk_out_header *buf, unsigned size);

/**
 * \brief write CCID device
 *
 * \param ccid  the CCID device
 * \param data  data buffer
 * \param size  data size
 * \return
 *      - (negative integer)    negative error number
 *      - other                 actual write size
 */
int fibo_usb_ccid_write(drvCCID_t *ccid, const struct ccid_bulk_in_header *data, unsigned size);

/**
 * \brief close CCID device
 *
 * \param ccid  the CCID device
 */
void fibo_usb_ccid_close(drvCCID_t *ccid);

#endif /* COMPONENTS_FIBOCOM_OPENCPU_OC_CCID_H */
