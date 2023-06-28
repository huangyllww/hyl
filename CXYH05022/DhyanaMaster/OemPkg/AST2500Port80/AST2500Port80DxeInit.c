//----------------------------------------------------------------------
//Include Files
//----------------------------------------------------------------------
#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiGenericSio.h>
#include <Library/AmiSioDxeLib.h>

EFI_STATUS AST2500_GPIO_Init(
    IN AMI_BOARD_INIT_PROTOCOL      *This,
    IN UINTN                        *Function,
    IN OUT VOID                     *ParameterBlock
);

SIO_DEVICE_INIT_DATA   DXE_GPIO_Init_Table_Port80_After_Active[] = {
    //-----------------------------
    //| Reg16 | AndData8  | OrData8  |
    //-----------------------------
    //OEM_TODO: Base on OEM board.
    //Program Global Configuration Registers.
    //These registers ask enter LDN== first.
    {0x07,0x00, AST2500_LDN_GPIO},
    {0x30, 0x7F, BIT7},
}; //DXE_GPIO_Init_Table_Port80_After_Active

/**
  This function provide GPIO initial routine in GenericSio.c

  @param  This                      AMI board initial protocol.
  @param  Function                  AMI sdl SIO initial routine.
  @param  ParameterBlock            SIO component initial routine parameters block...

  @retval  EFI_SUCCESS              Initial step sucessfully.
  @retval  EFI_INVALID_PARAMETER    Not find the initial step.
**/
EFI_STATUS AST2500_GPIO_Init_WithPort80(
    IN AMI_BOARD_INIT_PROTOCOL      *This,
    IN UINTN                        *Function,
    IN OUT VOID                     *ParameterBlock
)
{
    //Update Standard parameter block
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args=(AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
    SIO_INIT_STEP                   InitStep=(SIO_INIT_STEP)Args->InitStep;

    EFI_STATUS                      Status=EFI_SUCCESS;

    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_SIO_PARAM_SIG) return EFI_INVALID_PARAMETER;

    Status = AST2500_GPIO_Init(This, Function, ParameterBlock);

    switch (InitStep) {
    case isAfterActivate:
        ProgramIsaRegisterTable(AST2500_CONFIG_INDEX, AST2500_CONFIG_DATA,\
                                DXE_GPIO_Init_Table_Port80_After_Active,sizeof(DXE_GPIO_Init_Table_Port80_After_Active)/sizeof(SIO_DEVICE_INIT_DATA));
        break;

    } //switch

    return Status;
}

