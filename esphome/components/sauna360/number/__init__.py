import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    UNIT_MINUTE,
    UNIT_CELSIUS,
    ICON_TIMER,
    ENTITY_CATEGORY_CONFIG,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_TEMPERATURE,
    ICON_THERMOMETER,
)
from .. import (
    sauna360_ns,
    SAUNA360Component,
    CONF_SAUNA360_ID,
)

SAUNA360BathTimeNumber = sauna360_ns.class_("SAUNA360BathTimeNumber", number.Number)
SAUNA360BathTemperatureNumber = sauna360_ns.class_("SAUNA360BathTemperatureNumber", number.Number)

CONF_BATH_TIME = "bath_time"
CONF_BATH_TIME_DEFAULT = "bath_time_default"
CONF_BATH_TEMPERATURE = "bath_temperature"
CONF_BATH_TEMPERATURE_DEFAULT = "bath_temperature_default"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(SAUNA360Component),
        cv.Optional(CONF_BATH_TIME): number.number_schema(
            SAUNA360BathTimeNumber,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_MINUTE,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_TIMER,
        ).extend(
            {
                cv.Optional(CONF_BATH_TIME_DEFAULT): cv.float_range(min=1, max=360),
            }
        ),
        cv.Optional(CONF_BATH_TEMPERATURE): number.number_schema(
            SAUNA360BathTemperatureNumber,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_THERMOMETER,
        ).extend(
            {
                cv.Optional(CONF_BATH_TEMPERATURE_DEFAULT): cv.float_range(min=40, max=110),
            }
        ),
    }
)

async def to_code(config):
    sauna360_component = await cg.get_variable(config[CONF_SAUNA360_ID])
    if bath_time := config.get(CONF_BATH_TIME):
      n = await number.new_number(
        bath_time, min_value=5, max_value=360, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_bath_time_number(n))
      if CONF_BATH_TIME_DEFAULT in bath_time:
        cg.add(sauna360_component.set_bath_time_default_value(bath_time[CONF_BATH_TIME_DEFAULT]))
    if bath_temperature := config.get(CONF_BATH_TEMPERATURE):
      n = await number.new_number(
        bath_temperature, min_value=40, max_value=110, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_bath_temperature_number(n))
      if CONF_BATH_TEMPERATURE_DEFAULT in bath_temperature:
        cg.add(sauna360_component.set_bath_temperature_default_value(bath_temperature[CONF_BATH_TEMPERATURE_DEFAULT]))