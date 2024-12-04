from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import datetime
from esphome.const import (
    CONF_ID,
    CONF_ICON,
)

from .. import (
    sauna360_ns,
    SAUNA360Component,
    CONF_SAUNA360_ID,
)

SAUNA360DateTime = sauna360_ns.class_("SAUNA360DateTime", datetime.DateTimeEntity, cg.PollingComponent)
SAUNA360NotAllowedStartFromTime = sauna360_ns.class_("SAUNA360NotAllowedStartFromTime", datetime.TimeEntity, cg.PollingComponent)
SAUNA360NotAllowedStartUntilTime = sauna360_ns.class_("SAUNA360NotAllowedStartUntilTime", datetime.TimeEntity, cg.PollingComponent)

CONF_DATE_TIME = "date_time"
CONF_NOT_ALLOWED_START_FROM = "not_allowed_start_from"
CONF_NOT_ALLOWED_START_UNTIL = "not_allowed_start_until"

CONFIG_SCHEMA = cv.All(
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(SAUNA360DateTime),
            cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(SAUNA360Component),
            cv.Optional(CONF_DATE_TIME): datetime.datetime_schema(
                SAUNA360DateTime,
            ).extend(
                {
                    cv.Optional(CONF_ICON, default="mdi:calendar-clock"): cv.icon,
                }
            ),
            cv.Optional(CONF_NOT_ALLOWED_START_FROM): datetime.time_schema(
                SAUNA360NotAllowedStartFromTime,
            ).extend(
                {
                    cv.Optional(CONF_ICON, default="mdi:timelapse"): cv.icon,
                }
            ),
            cv.Optional(CONF_NOT_ALLOWED_START_UNTIL): datetime.time_schema(
                SAUNA360NotAllowedStartUntilTime,
            ).extend(
                {
                    cv.Optional(CONF_ICON, default="mdi:timelapse"): cv.icon,
                }
            ),
        }
    ),
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if CONF_DATE_TIME in config:
        datetime_struct = cg.StructInitializer(
            cg.ESPTime,
        )
        datetime_var = await datetime.new_datetime(config[CONF_DATE_TIME])
        cg.add(datetime_var.set_initial_value(datetime_struct))
    if CONF_NOT_ALLOWED_START_FROM in config:
        time_struct = cg.StructInitializer(
            cg.ESPTime,
        )
        time_var = await datetime.new_datetime(config[CONF_NOT_ALLOWED_START_FROM])
        cg.add(time_var.set_initial_value(time_struct))
    if CONF_NOT_ALLOWED_START_UNTIL in config:
        time_struct = cg.StructInitializer(
            cg.ESPTime,
        )
        time_var = await datetime.new_datetime(config[CONF_NOT_ALLOWED_START_UNTIL])
        cg.add(time_var.set_initial_value(time_struct))