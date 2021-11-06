
/// addr assignments
#define HUB_ADDR 0x40
#define UNASSIGNED_SHELF_ADDR 0x4F
#define ASSIGNED_SHELF_BASE_ADDR 0x50
#define ASSIGNED_SHELF_MAX_ADDR 0x7f

/// Hub Register map
/// >>> 0xff : enroll, start enrolling new device
#define HUB_ENROL_REQ 0x00

/// Shelf Register Map (working addrs)
/// >>> UID / Device data
#define SHELF_UID_B0 0xb0        /// r/w : uid byte 0
#define SHELF_UID_B1 0xb1        /// r/w : uid byte 1
#define SHELF_UID_B2 0xb2        /// r/w : uid byte 2
#define SHELF_UID_B3 0xb3        /// r/w : uid byte 3
#define SHELF_DRAWERS_WIDE 0xa0  /// r   : width of shelf (in drawers)
#define SHELF_DRAWERS_HIGH 0xa1  /// r   : height of shelf (in drawers)
#define SHELF_I2C_ADDR 0xad      /// w   : device address (init=unassigned addr)
/// >>> drawer addressing
#define SHELF_UID_ 0xc0  /// r/w : current drawer horizontal index
#define SHELF_UID_ 0xc1  /// r/w : current drawer vertical index
/// >>> command addr
#define SHELF_CMD 0xff  /// w : command reg (
///     - 0x00 drawer off
///     - 0x01 drawer on
///     - 0xf0 all off
///     - 0xf1 all on
