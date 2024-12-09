import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import datetime
from esphome.const import (
    CONF_ICON,
)

from .. import (
    sauna360_ns,
    SAUNA360Component,
    CONF_SAUNA360_ID,
)

SAUNA360DateTime = sauna360_ns.class_("SAUNA360DateTime", datetime.DateTimeEntity, cg.Component)
SAUNA360NotAllowedStartFromTime = sauna360_ns.class_("SAUNA360NotAllowedStartFromTime", datetime.TimeEntity, cg.Component)
SAUNA360NotAllowedStartUntilTime = sauna360_ns.class_("SAUNA360NotAllowedStartUntilTime", datetime.TimeEntity, cg.Component)

CONF_NOT_ALLOWED_START_FROM = "not_allowed_start_from"
CONF_NOT_ALLOWED_START_UNTIL = "not_allowed_start_until"

CONFIG_SCHEMA = datetime.datetime_schema(SAUNA360DateTime).extend(
    {
        cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(SAUNA360Component),
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
).extend(cv.COMPONENT_SCHEMA).extend(
    {
        cv.Optional(CONF_ICON, default="mdi:calendar-clock"): cv.icon,
    }
)

async def to_code(config):
    var = await datetime.new_datetime(config)
    sauna360 = await cg.get_variable(config[CONF_SAUNA360_ID])
    await cg.register_parented(var, config[CONF_SAUNA360_ID])
    await cg.register_component(var, config)
    if CONF_NOT_ALLOWED_START_FROM in config:
        time_struct = cg.StructInitializer(
            cg.ESPTime,
            ("minute", 0),
            ("hour", 0),
        )
        time_var = await datetime.new_datetime(config[CONF_NOT_ALLOWED_START_FROM])
        cg.add(time_var.set_initial_value(time_struct))
        cg.add(sauna360.register_listener(time_var))
    if CONF_NOT_ALLOWED_START_UNTIL in config:
        time_struct = cg.StructInitializer(
            cg.ESPTime,
            ("minute", 0),
            ("hour", 0),
        )
        time_var = await datetime.new_datetime(config[CONF_NOT_ALLOWED_START_UNTIL])
        cg.add(time_var.set_initial_value(time_struct))
        cg.add(sauna360.register_listener(time_var))
    cg.add(sauna360.register_listener(var))