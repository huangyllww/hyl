/*************************************************************************\
*  Module Name: iolib.h
*
*  Descriprtion:
*
*  Date:
\**************************************************************************/
/* IOType Define */
#define	NormalIO	0
#define	IndexIO		1

/* ProtoType Type */
VOID SetReg (USHORT usPORT, UCHAR jMask, UCHAR jData);
VOID SetReg2 (USHORT usPORT, UCHAR jData);
UCHAR GetReg (USHORT usPORT, UCHAR jMask);
UCHAR GetReg2 (USHORT usPORT);
VOID SetIndexReg (USHORT usPORT, UCHAR jIndex, UCHAR jMask, UCHAR jData);
VOID SetIndexReg2 (USHORT usPORT, UCHAR jIndex, UCHAR jData);
UCHAR GetIndexReg (USHORT usPORT, UCHAR jIndex, UCHAR jMask);
UCHAR GetIndexReg2 (USHORT usPORT, UCHAR jIndex);
VOID Delay15us (VOID);

