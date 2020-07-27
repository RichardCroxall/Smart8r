#pragma once

/* when assert traps, it stops the program. When we only want a warning, use this */

#define warning(testvalue) {if (!(testvalue)) {logging.logWarn("warning at line %d in file %s\n", __LINE__, __FILE__);}}
