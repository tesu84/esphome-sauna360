import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_DURATION,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_MINUTE,
)

from .. import (
    sauna360_ns,
    SAUNA360Component,
    CONF_SAUNA360_ID,
)

SAUNA360Sensor = sauna360_ns.class_("SAUNA360Sensor", sensor.Sensor, cg.Component) 

CONF_CURRENT_TEMPERATURE = "current_temperature"
CONF_SETTING_TEMPERATURE = "setting_temperature"
CONF_REMAINING_TIME = "remaining_time"
CONF_HUMIDITY_SETTING = "humidity_setting"
CONF_HUMIDITY_PERCENTAGE = "humidity_percentage"
CONF_SETTING_BATH_TIME = "setting_bath_time"

CONFIG_SCHEMA = cv.All(
    cv.COMPONENT_SCHEMA.extend(
        {
          cv.GenerateID(): cv.declare_id(SAUNA360Sensor),
          cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(SAUNA360Component),
          cv.Optional(CONF_CURRENT_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
            ),
          cv.Optional(CONF_SETTING_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
            ),
          cv.Optional(CONF_REMAINING_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTE,
            device_class=DEVICE_CLASS_DURATION,
            state_class=STATE_CLASS_MEASUREMENT,
            ),
          cv.Optional(CONF_HUMIDITY_SETTING): sensor.sensor_schema(
            unit_of_measurement="",
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            ),
          cv.Optional(CONF_HUMIDITY_PERCENTAGE): sensor.sensor_schema(
            unit_of_measurement="%",
            accuracy_decimals=0,
            device_class="humidity",
            state_class=STATE_CLASS_MEASUREMENT,
            ),
          cv.Optional(CONF_SETTING_BATH_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTE,
            device_class=DEVICE_CLASS_DURATION,
            state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    ),
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if CONF_CURRENT_TEMPERATURE in config:
      sens = await sensor.new_sensor(config[CONF_CURRENT_TEMPERATURE])
      cg.add(var.set_temperature_sensor(sens))
    if CONF_SETTING_TEMPERATURE in config:
      sens = await sensor.new_sensor(config[CONF_SETTING_TEMPERATURE])
      cg.add(var.set_temperature_setting_sensor(sens))
    if CONF_REMAINING_TIME in config:
      sens = await sensor.new_sensor(config[CONF_REMAINING_TIME])
      cg.add(var.set_remaining_time_sensor(sens))
    sauna360 = await cg.get_variable(config[CONF_SAUNA360_ID])
    if CONF_HUMIDITY_SETTING in config:
      sens = await sensor.new_sensor(config[CONF_HUMIDITY_SETTING])
      cg.add(var.set_humidity_sensor(sens))
    if CONF_HUMIDITY_PERCENTAGE in config:
      sens = await sensor.new_sensor(config[CONF_HUMIDITY_PERCENTAGE])
      cg.add(var.set_humidity_percentage_sensor(sens))
    cg.add(sauna360.register_listener(var))
    if CONF_SETTING_BATH_TIME in config:
      sens = await sensor.new_sensor(config[CONF_SETTING_BATH_TIME])
      cg.add(var.set_bath_time_setting_sensor(sens))
    cg.add(sauna360.register_listener(var))