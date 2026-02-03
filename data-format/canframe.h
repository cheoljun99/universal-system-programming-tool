#pragma once

#include <cstdint>
#include <cstddef>

/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG 0x80000000U /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000U /* remote transmission request */
#define CAN_ERR_FLAG 0x20000000U /* error message frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK 0x000007FFU /* standard frame format (SFF) */
#define CAN_EFF_MASK 0x1FFFFFFFU /* extended frame format (EFF) */
#define CAN_ERR_MASK 0x1FFFFFFFU /* omit EFF, RTR, ERR flags */
#define CANXL_PRIO_MASK CAN_SFF_MASK /* 11 bit priority mask */

#define CAN_SFF_ID_BITS		11
#define CAN_EFF_ID_BITS		29
#define CANXL_PRIO_BITS		CAN_SFF_ID_BITS

/*
 * Controller Area Network Error Message Frame Mask structure
 *
 * bit 0-28	: error class mask (see include/uapi/linux/can/error.h)
 * bit 29-31	: set to zero
 */
/* CAN payload length and DLC definitions according to ISO 11898-1 */
#define CAN_MAX_DLC 8
#define CAN_MAX_RAW_DLC 15
#define CAN_MAX_DLEN 8

/* CAN FD payload length and DLC definitions according to ISO 11898-7 */
#define CANFD_MAX_DLC 15
#define CANFD_MAX_DLEN 64

/*
 * CAN XL payload length and DLC definitions according to ISO 11898-1
 * CAN XL DLC ranges from 0 .. 2047 => data length from 1 .. 2048 byte
 */
#define CANXL_MIN_DLC 0
#define CANXL_MAX_DLC 2047
#define CANXL_MAX_DLC_MASK 0x07FF
#define CANXL_MIN_DLEN 1
#define CANXL_MAX_DLEN 2048

#define CANFD_BRS 0x01 /* bit rate switch (second bitrate for payload data) */
#define CANFD_ESI 0x02 /* error state indicator of the transmitting node */
#define CANFD_FDF 0x04 /* mark CAN FD for dual use of struct canfd_frame */

#define CANXL_XLF 0x80 /* mandatory CAN XL frame flag (must always be set!) */
#define CANXL_SEC 0x01 /* Simple Extended Content (security/segmentation) */

/* the 8-bit VCID is optionally placed in the canxl_frame.prio element */
#define CANXL_VCID_OFFSET 16 /* bit offset of VCID in prio element */
#define CANXL_VCID_VAL_MASK 0xFFUL /* VCID is an 8-bit value */
#define CANXL_VCID_MASK (CANXL_VCID_VAL_MASK << CANXL_VCID_OFFSET)


struct CanFrame final {
    uint32_t canId_; // CAN ID + EFF/RTR/ERR flags
    uint8_t  len_;    // DLC (0~8)
    uint8_t  res0_;   // reserved
    uint8_t  res1_;   // reserved
    uint8_t  len8_dlc_;   // len8_dlc
    uint8_t data_[CAN_MAX_DLEN] __attribute__((aligned(8)));
};
typedef CanFrame* PCanHdr;
struct CanFdFrame final {
    uint32_t canId_; // CAN ID + EFF/RTR/ERR flags
    uint8_t len_; // DLC
    uint8_t flags_; // CAN FD flags (BRS/ESI 등)
    uint8_t res0_; // reserved
    uint8_t res1_; // reserved
    uint8_t data_[CANFD_MAX_DLEN] __attribute__((aligned(8)));;
};
typedef CanFdFrame* PCanFdHdr;
struct CanXlFrame {
    uint32_t prio_;  /* 11 bit priority for arbitration (canid_t) */
    uint8_t flags_; /* additional flags for CAN XL */
    uint8_t sdt_;   /* SDU (service data unit) type */
    uint16_t len_;   /* frame payload length in byte */
    uint32_t af_;    /* acceptance field */
    uint8_t data_[CANXL_MAX_DLEN];
};
typedef CanXlFrame* PCanXlHdr;

#define CAN_MTU		(sizeof(CanFrame))
#define CANFD_MTU	(sizeof(CanFdFrame))
#define CANXL_MTU	(sizeof(CanXlFrame))
#define CANXL_HDR_SIZE	(offsetof(CanXlFrame, data_))
#define CANXL_MIN_MTU	(CANXL_HDR_SIZE + 64)
#define CANXL_MAX_MTU	CANXL_MTU

union CanFrameUnion {
    struct CanFrame cc;
    struct CanFdFrame fd;
    struct CanXlFrame xl;
};

