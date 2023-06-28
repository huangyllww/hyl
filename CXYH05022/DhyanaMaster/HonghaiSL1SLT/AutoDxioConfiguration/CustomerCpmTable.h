#ifndef __CUSTOMER_CPM_TABLE_H__
#define __CUSTOMER_CPM_TABLE_H__
typedef struct {
    DXIO_PORT_DESCRIPTOR*  DxioPortConfig;
    UINT8                  DxioPortCount;
}CUSTOMER_DXIO_PLAN;

extern CUSTOMER_DXIO_PLAN  Die0Type0DxioConfigPlan[];
extern CUSTOMER_DXIO_PLAN  Die0Type1DxioConfigPlan[];
extern CUSTOMER_DXIO_PLAN  Die1Type0DxioConfigPlan[];
extern CUSTOMER_DXIO_PLAN  Die1Type1DxioConfigPlan[];
extern CUSTOMER_DXIO_PLAN  Die2Type0DxioConfigPlan[];
extern CUSTOMER_DXIO_PLAN  Die2Type1DxioConfigPlan[];
extern CUSTOMER_DXIO_PLAN  Die3Type0DxioConfigPlan[];
extern CUSTOMER_DXIO_PLAN  Die3Type1DxioConfigPlan[];
#endif