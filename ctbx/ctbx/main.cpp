#include "./ctbx.h"

namespace app = cq::app; 
namespace event = cq::event; 

CQ_INITIALIZE(CQ_APP_ID);

CQ_MAIN{
cq::config.convert_unicode_emoji = true; 

app::on_enable = ctbx::cqevent::bot_on_enable;

event::on_group_msg = ctbx::cqevent::bot_receive_groupmessage;

app::on_disable = ctbx::cqevent::bot_on_disable;

app::on_coolq_exit = ctbx::cqevent::bot_on_exit;
}