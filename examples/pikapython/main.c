#include <stdlib.h>
#include "board.h"
#include "ff.h"
#include "log.h"
#include "pikaScript.h"
#include "vlibc_stdio.h"

int main(void) {
    board_init();
    pikaScriptInit();
}
