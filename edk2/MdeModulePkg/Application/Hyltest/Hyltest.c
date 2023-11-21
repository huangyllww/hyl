#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/ShellParameters.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseLib.h>
// String token ID of help message text.

// Shell supports to find help message in the resource section of an application image if
// .MAN file is not found. This global variable is added to make build tool recognizes
// that the help string is consumed by user and then build tool will add the string into
// the resource section. Thus the application can use '-?' option to show help message in
// Shell.
//

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
typedef struct 
{
  OUT UINT32 EAX_Reg;
  OUT UINT32 EBX_Reg;
  OUT UINT32 ECX_Reg;
  OUT UINT32 EDX_Reg;
  /* data */
}CPUID_DATA;
UINTN   Argc;
CHAR16  **Argv;
CPUID_DATA Cpuid;

VOID
ReadCpuid( )
{ 
   
   AsmCpuid (0x80000000, &Cpuid.EAX_Reg, &Cpuid.EBX_Reg, &Cpuid.ECX_Reg, &Cpuid.EDX_Reg);
 
}

VOID printStringFromUINT32(UINT32 value)
{
  unsigned char byte1=(value >> 24) &0xff;
  unsigned char byte2=(value >> 16) &0xff;
  unsigned char byte3=(value >> 8) &0xff;
  unsigned char byte4= value  &0xff;
  char str[5];
  str[0]=byte4;
  str[1]=byte3;
  str[2]=byte2;
  str[3]=byte1;
  Print (L"%c%c%c%c.",str[0], str[1], str[2] ,str[3]);
}
EFI_STATUS
GetArg (
  VOID
  )
{
  
  EFI_STATUS                     Status;
  EFI_SHELL_PARAMETERS_PROTOCOL  *ShellParameters;

  Status = gBS->HandleProtocol (
                  gImageHandle,
                  &gEfiShellParametersProtocolGuid,
                  (VOID **)&ShellParameters
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Argc = ShellParameters->Argc;
  Argv = ShellParameters->Argv;
  return EFI_SUCCESS;
}
//
// Argv: [-h]/[-H] display helpmenu 
//
VOID 
HelpMenu ()
{
    Print (L"---Argv----------------Func---\n");
    Print (L"[-V]/[-v]           Version \n");
    Print (L"[-H]/[-h]           Help info\n");
    Print (L"[-A]/[-a]           Add   \n");
    Print (L"[-B]/[-b]           Hellow world \n");
}
VOID 
ShowVersion()
{  
  Print (L"Hyl-test application\n");
  Print (L"Version:1.0.0\n");
}
VOID 
PrintArg() {
     Print (L"Argc :%d \n", Argc);
     Print (L"Argv[0]: %s\n",(CHAR16 *)Argv[0]);
     Print (L"Argv[1]: %s\n",(CHAR16 *)Argv[1]);
     Print (L"Argv[2]: %s\n",(CHAR16 *)Argv[2]);
  }

VOID
Helloworld()
{  
  Print (L"Hellow World\n");
}
EFI_STATUS
EFIAPI
Hyltestentry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
   EFI_STATUS     Status;
   UINTN num ;
   ReadCpuid(Cpuid);
   Print (L"        CPUID:"); 
   printStringFromUINT32(Cpuid.EBX_Reg);
   printStringFromUINT32(Cpuid.EDX_Reg);
   printStringFromUINT32(Cpuid.ECX_Reg);
   Status = GetArg ();
   DEBUG ((DEBUG_ERROR, "Hyltestentry---------in!\n"));
    
   //ReadCpuid();
  if (EFI_ERROR (Status)) {
    Print (L"Please use UEFI SHELL to run this application!\n", Status);
    return Status;
  }
  if (Argc != 3) {
    Print (L"Please Input Correct number of parameters ,It Must be 3 parameter\n", Status);
    HelpMenu();
    return EFI_UNSUPPORTED;
  }
  DEBUG ((DEBUG_ERROR, "---------Argc:%d\n", Argc));
  DEBUG ((DEBUG_ERROR, "---------Argv[0]:%s\n", (CHAR16 *)Argv[0]));
  DEBUG ((DEBUG_ERROR, "---------Argv[1]:%s\n", (CHAR16 *)Argv[1]));
  DEBUG ((DEBUG_ERROR, "---------Argv[2]:%s\n", (CHAR16 *)Argv[2]));

  if ((StrCmp (Argv[1], L"-H") == 0) || (StrCmp (Argv[1], L"-h") == 0) ||(StrCmp (Argv[2], L"-h") == 0)||(StrCmp (Argv[2], L"-H") == 0)) {
       HelpMenu ();
  } 
  if ((StrCmp (Argv[1], L"-V") == 0) || (StrCmp (Argv[1], L"-v") == 0) ||(StrCmp (Argv[2], L"-V") == 0)||(StrCmp (Argv[2], L"-v") == 0)) {
       ShowVersion ();
  } 
   if ((StrCmp (Argv[1], L"-A") == 0) || (StrCmp (Argv[1], L"-a") == 0)) {
       num = StrDecimalToUintn( (CHAR16 *)Argv[2]);
       num=num*num;
       Print (L"Argv[2]: %d\n",num);
       DEBUG ((DEBUG_ERROR, "---AsciiStrDecimalToUintn----num--Argv[2]:%d\n", num));
  } 
   if ((StrCmp (Argv[1], L"-B") == 0) || (StrCmp (Argv[1], L"-b") == 0) ||(StrCmp (Argv[2], L"-b") == 0)||(StrCmp (Argv[2], L"-B") == 0)) {
       Helloworld ();
       PrintArg();
  } 
  return EFI_SUCCESS;


}