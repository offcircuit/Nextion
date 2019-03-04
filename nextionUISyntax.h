#define NEXTION_CMD_INVALID_INSTRUCTION                         0x00
#define NEXTION_CMD_INSTRUCTION_SUCESSFUL                       0x01
#define NEXTION_CMD_INVALID_COMPONENT_ID                        0x02
#define NEXTION_CMD_INVALID_PAGE_ID                             0x03
#define NEXTION_CMD_INVALID_PICTURE_ID                          0x04
#define NEXTION_CMD_INVALID_FONT_ID                             0x05
#define NEXTION_CMD_INVALID_BAUD_RATE                           0x11
#define NEXTION_CMD_INVALID_WAVEFORM_ID_OR_CHANNEL              0x12
#define NEXTION_CMD_INVALID_VARIABLE_NAME_OR_ATTRIBUTE          0x1A
#define NEXTION_CMD_INVALID_VARIABLE_OPERATION                  0x1B
#define NEXTION_CMD_ATTRIBUTE_ASSIGN_FAILED                     0x1C
#define NEXTION_CMD_EEPROM_FAILED                               0x1D
#define NEXTION_CMD_INVALID_PARAMETERS_QUANTITY                 0x1E
#define NEXTION_CMD_IO_OPERATION_FAILED                         0x1F
#define NEXTION_CMD_INVALID_ESCAPE_CHARATER                     0x20
#define NEXTION_CMD_VARIABLE_NAME_TOO_LONG                      0x23

#define NEXTION_CMD_STARTUP                                     0x0006//
#define NEXTION_CMD_SERIAL_BUFFER_OVERFLOW                      0x2404//
#define NEXTION_CMD_TOUCH_EVENT                                 0x6507//
#define NEXTION_CMD_CURRENT_PAGE                                0x6605
#define NEXTION_CMD_TOUCH_COORDINATE_AWAKE                      0x6709//
#define NEXTION_CMD_TOUCH_COORDINATE_SLEEP                      0x6809//
#define NEXTION_CMD_STRING_DATA_ENCLOSED                        0x7009
#define NEXTION_CMD_NUMERIC_DATA_ENCLOSED                       0x7108
#define NEXTION_CMD_AUTO_ENTER_SLEEP                            0x8604//
#define NEXTION_CMD_AUTO_ENTER_WAKEUP                           0x8704//
#define NEXTION_CMD_READY                                       0x8804//
#define NEXTION_CMD_START_MICROSD_UPDATE                        0x8904//
#define NEXTION_CMD_TRANSPARENT_DATA_END                        0xFD04
#define NEXTION_CMD_TRANSPARENT_DATA_READY                      0xFE04

#define NEXIION_OBJ_PAGE                                        0x0000
#define NEXIION_OBJ_TEXT                                        0x0000
#define NEXIION_OBJ_SCRTEXT                                     0x0000
#define NEXIION_OBJ_NUMBER                                      0x0000
#define NEXIION_OBJ_BUTTON                                      0x0000
#define NEXIION_OBJ_PROGRESSBAR                                 0x0000
#define NEXIION_OBJ_PICTURE                                     0x0000
#define NEXIION_OBJ_CROP                                        0x0000
#define NEXIION_OBJ_HOTSPOT                                     0x0000
#define NEXIION_OBJ_GAUGE                                       0x0000
#define NEXIION_OBJ_WAVEFORM                                    0x0000
#define NEXIION_OBJ_SLIDER                                      0x0000
#define NEXIION_OBJ_TIMER                                       0x0000
#define NEXIION_OBJ_VARIABLE                                    0x0000
#define NEXIION_OBJ_DSBUTTON                                    0x0000
#define NEXIION_OBJ_CHECKBOX                                    0x0000
#define NEXIION_OBJ_RADIO                                       0x0000
#define NEXIION_OBJ_QRCODE                                      0x0000

struct nexComponent {
  int8_t page, id;
};

enum nexEvent {
  NEXTION_EVENT_RELEASE,
  NEXTION_EVENT_PRESS
};

struct nexTouch {
  int8_t page, id;
  nexEvent event;
};

typedef struct nexCommand {
  uint32_t code = 0UL;
  nexEvent event = NULL;
};
