
//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <Token.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <AmiPeiLib.h>
#include <Setup.h>

static EFI_GUID gSetupGuid = SETUP_GUID;

VOID lpc_read(
    IN    UINT32        Address,
    OUT   UINT32         *Value
);

VOID lpc_write(
    IN    UINT32        Address,
    IN    UINT32         Vaule
);


EFI_STATUS
EFIAPI
ReadOnlyVariable2PpiNotifyCallback(
    IN EFI_PEI_SERVICES           **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
    IN VOID                       *Ppi
);

STATIC
EFI_PEI_NOTIFY_DESCRIPTOR        gNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiReadOnlyVariable2PpiGuid,
  ReadOnlyVariable2PpiNotifyCallback
};

EFI_STATUS
EFIAPI
ReadOnlyVariable2PpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
    EFI_STATUS                          Status;
    UINT32                              Data = 0;
    SETUP_DATA                          mSetupData;
    UINTN                               Size = sizeof(SETUP_DATA);
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable = (EFI_PEI_READ_ONLY_VARIABLE2_PPI *)Ppi;

    Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable,
            L"Setup",
            &gSetupGuid,
            NULL,
            &Size,
            &mSetupData );
    
    if(EFI_ERROR(Status))
    {
        return Status;
    }

    if(mSetupData.BmcVgaMemSize <= 3)
    {
        //SCU70: Hardware Strap Register
        //BIT[3:2] 00b - 8MB, 01b - 16MB, 10b - 32MB, 11b - 64MB
        //The write operation to 0x1E6E2070 only can set to '1'. To clear to '0', it must write '1' to 0x1E6E207C (write 1 clear).
        lpc_write(AST2500_SCU_BASE + 0x00, 0x1688A8A8); //unlock scu register

        lpc_read(AST2500_SCU_BASE + 0x70, &Data);
        lpc_write(AST2500_SCU_BASE + 0x70, Data | 0x0000000C);

        Data = (((~mSetupData.BmcVgaMemSize) & 0x03) << 2);
        lpc_write(AST2500_SCU_BASE + 0x7C, Data);
    }
    return EFI_SUCCESS;
}

EFI_STATUS OemBmcVgaConfSioHookLibConstructor(
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS                    Status;
    Status = (*PeiServices)->NotifyPpi(PeiServices, &gNotifyList);

    return EFI_SUCCESS;
}
