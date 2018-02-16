#pragma once

#include "unified_message.h"

extern int HANDLER_PASS;
extern int HANDLER_TERMINATE;

int tg_pre_handler(UnifiedMessage message);
int qq_pre_handler(UnifiedMessage message);

int showid_handler(UnifiedMessage message);
int ali_handler(UnifiedMessage message);