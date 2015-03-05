#define KEY1_OFFSET 0
#define KEY2_OFFSET 3
#define KEY3_OFFSET 5
#define KEY4_OFFSET 6
#define KEY5_OFFSET 7
#define KEY6_OFFSET 11

#define KEY_NUM 6

struct key_ctrl {
    int key_num;
    int key_state;
};

#define IOCTL_CMD_MAKE

#ifdef IOCTL_CMD_MAKE

#define KEY_DEV_MAGIC 'w'
/* #define KEY_SET_STATE_IOCTL_CMD  (_IOW(KEY_DEV_MAGIC, 0x20,  struct key_ctrl)) */
#define KEY_GET_STATE_IOCTL_CMD  (_IOR(KEY_DEV_MAGIC, 0x21, struct key_ctrl))

#else

#define KEY_SET_STATE_IOCTL_CMD   1
#define KEY_GET_STATE_IOCTL_CMD   2
#endif

#define KEY_PRESSED      1
#define KEY_NOT_PRESSED  0

