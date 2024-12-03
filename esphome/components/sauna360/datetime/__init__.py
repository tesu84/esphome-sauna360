from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import datetime
from esphome.const import (
    CONF_ID,
)

from .. import (
    sauna360_ns,
    SAUNA360Component,
    CONF_SAUNA360_ID,
)

SAUNA360DateTime = sauna360_ns.class_("SAUNA360DateTime", datetime.DateTimeEntity, cg.PollingComponent)

CONF_DATE_TIME = "date_time"

CONFIG_SCHEMA = cv.All(
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(SAUNA360DateTime),
            cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(SAUNA360Component),
            cv.Optional(CONF_DATE_TIME): datetime.datetime_schema(
                SAUNA360DateTime,
            ),
        }
    ),
)

async def to_code(config):
    #var = await datetime.new_datetime(config)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if CONF_DATE_TIME in config:
        datetime_struct = cg.StructInitializer(
            cg.ESPTime,
        )
        cg.add(var.set_initial_value(datetime_struct))
    await cg.register_component(var, config)
    #sauna360 = await cg.get_variable(config[CONF_SAUNA360_ID])
    #cg.add(sauna360.register_listener(var))