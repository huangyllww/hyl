IFDEF EFIx64
ELSE
    .686
    .model  flat
ENDIF

    .code

IFDEF EFIx64
include sha1transform_x64.inc
ELSE
include sha1transform_ia32.inc
ENDIF


END
