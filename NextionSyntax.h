#define NEXTION_NUMERIC_CONVERT(b1, b2, b3, b4)({(uint32_t)b4 << 24 | (uint32_t)b3 << 16 | (uint32_t)b2 << 8 | b1;})

#define NEXTION_UART_INSTRUCTION(s) ({String(s) + char(0xFF) + char(0xFF) + char(0xFF);})
#define NEXTION_UART_SIZE 250

#define NEXTION_CMD_INVALID_INSTRUCTION                         0x00 //RECEIPT
#define NEXTION_CMD_INSTRUCTION_SUCESSFUL                       0x01 //RECEIPT
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

#define NEXTION_CMD_STARTUP                                     0x00 // LISTEN
#define NEXTION_CMD_SERIAL_BUFFER_OVERFLOW                      0x24 // LISTEN
#define NEXTION_CMD_TOUCH_EVENT                                 0x65 // LISTEN
#define NEXTION_CMD_CURRENT_PAGE                                0x66 // CURRENT
#define NEXTION_CMD_TOUCH_COORDINATE_AWAKE                      0x67 // LISTEN
#define NEXTION_CMD_TOUCH_COORDINATE_SLEEP                      0x68 // LISTEN
#define NEXTION_CMD_STRING_DATA_ENCLOSED                        0x70 // GETATTRIBUTE
#define NEXTION_CMD_NUMERIC_DATA_ENCLOSED                       0x71 // GETATTRIBUTE
#define NEXTION_CMD_AUTO_ENTER_SLEEP                            0x86 // LISTEN
#define NEXTION_CMD_AUTO_ENTER_WAKEUP                           0x87 // LISTEN
#define NEXTION_CMD_READY                                       0x88 // LISTEN
#define NEXTION_CMD_START_MICROSD_UPDATE                        0x89 // LISTEN
#define NEXTION_CMD_TRANSPARENT_DATA_END                        0xFD
#define NEXTION_CMD_TRANSPARENT_DATA_READY                      0xFE

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

struct nextionComponent {
  int8_t page, id;
};

enum nextionEvent {
  NEXTION_EVENT_RELEASE,
  NEXTION_EVENT_PRESS
};

struct nextionTouch {
  int8_t page, id;
  nextionEvent event;
};
