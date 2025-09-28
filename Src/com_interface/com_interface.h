
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

typedef struct {
    
}com_interface_TypeDef;

typedef enum {
    COM_INTERFACE_OK = 0,
    COM_INTERFACE_ERROR = -1,
    COM_INTERFACE_BUSY = -2,
    COM_INTERFACE_TIMEOUT = -3
}com_interface_StatusTypeDef;