#!/usr/bin/env python3
"""Count-checked idle TLS checks in protocol connection I/O loops."""
from pathlib import Path
import sys

source = Path(sys.argv[1])
text = source.read_text()


def replace(old, new):
    global text
    if text.count(old) != 1:
        raise RuntimeError('Review TLS idle-loop integration: ' + repr(old))
    text = text.replace(old, new)


if sys.argv[2] == 'mms':
    replace('#include "stack_config.h"', '#include "stack_config.h"\n#if (CONFIG_MMS_SUPPORT_TLS == 1)\n#include "iec62351_tls.h"\n#endif')
    replace('    CotpConnection_flushBuffer(self->cotpConnection);', '''#if (CONFIG_MMS_SUPPORT_TLS == 1)
    if (self->tlsSocket && !CCLI_TLS_tick(self->tlsSocket)) {
        self->state = ISO_CON_STATE_STOPPED;
        return;
    }
#endif
    CotpConnection_flushBuffer(self->cotpConnection);''')
    replace('        IsoConnection_callTickHandler(self);', '        IsoConnection_callTickHandler(self);\n        if (self->state != ISO_CON_STATE_RUNNING) goto exit_function;')
else:
    # Configuration macros are defined by lib60870's includes, not by CMake.
    text += '\n'  # Preserve the source as a conventional text file.
    anchor = '#include "tls_socket.h"'
    replace(anchor, anchor + '\n#include "iec62351_tls.h"')
    replace('        if (Handleset_waitReady(self->handleSet, socketTimeout))', '''#if (CONFIG_CS104_SUPPORT_TLS == 1)
        if (self->tlsSocket && !CCLI_TLS_tick(self->tlsSocket)) break;
#endif
        if (Handleset_waitReady(self->handleSet, socketTimeout))''')
    replace('MasterConnection_executePeriodicTasks(MasterConnection self)\n{', '''MasterConnection_executePeriodicTasks(MasterConnection self)
{
#if (CONFIG_CS104_SUPPORT_TLS == 1)
    if (self->tlsSocket && !CCLI_TLS_tick(self->tlsSocket)) {
        self->isRunning = false;
        return;
    }
#endif''')
source.write_text(text, encoding='utf-8')
