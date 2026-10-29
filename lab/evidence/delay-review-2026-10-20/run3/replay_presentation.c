#include "iso_presentation.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    if (argc != 2) return 2;
    FILE* input = fopen(argv[1], "rb");
    if (!input) return 2;
    unsigned char data[8192];
    int size = (int)fread(data, 1, sizeof(data), input);
    fclose(input);
    IsoPresentation presentation;
    IsoPresentation_init(&presentation);
    ByteBuffer message;
    ByteBuffer_wrap(&message, data, size, sizeof(data));
    int accepted = IsoPresentation_parseConnect(&presentation, &message);
    printf("presentation_parse=%s payload_size=%d acse_context=%d mms_context=%d\n",
           accepted ? "ACCEPT" : "REJECT", size,
           presentation.acseContextId, presentation.mmsContextId);
    return accepted ? 0 : 1;
}
