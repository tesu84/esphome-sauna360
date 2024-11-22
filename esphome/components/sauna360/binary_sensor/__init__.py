import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_HEAT,
    DEVICE_CLASS_LIGHT,
    DEVICE_CLASS_SAFETY,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from .. import (
    sauna360_ns,
    SAUNA360Component,
    CONF_SAUNA360_ID,
)

SAUNA360BinarySensor = sauna360_ns.class_("SAUNA360BinarySensor", binary_sensor.BinarySensor, cg.Component)

CONF_HEATER_STATUS = "heater_status"
CONF_LIGHT_STATUS = "light_status"
CONF_READY_STATUS = "ready_status"
CONF_RELAY_X3_X4_STATUS = "relay_x3_x4_status"
CONF_RELAY_X5_X6_STATUS = "relay_x5_x6_status"
CONF_RELAY_X7_X8_STATUS = "relay_x7_x8_status"
CONF_RELAY_X9_X10_STATUS = "relay_x9_x10_status"
CONF_RELAY_X11_X12_STATUS = "relay_x11_x12_status"
CONF_RELAY_X13_X14_STATUS = "relay_x13_x14_status"
CONF_RELAY_X15_X16_STATUS = "relay_x15_x16_status"
CONF_RELAY_X17_X18_STATUS = "relay_x17_x18_status"

CONFIG_SCHEMA = cv.All(
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(SAUNA360BinarySensor),
            cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(SAUNA360Component),
            cv.Optional(CONF_HEATER_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_HEAT
            ),
            cv.Optional(CONF_LIGHT_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_LIGHT
            ),
            cv.Optional(CONF_READY_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_SAFETY
            ),
            cv.Optional(CONF_RELAY_X3_X4_STATUS): binary_sensor.binary_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),
            cv.Optional(CONF_RELAY_X5_X6_STATUS): binary_sensor.binary_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),
            cv.Optional(CONF_RELAY_X7_X8_STATUS): binary_sensor.binary_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),
            cv.Optional(CONF_RELAY_X9_X10_STATUS): binary_sensor.binary_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),
            cv.Optional(CONF_RELAY_X11_X12_STATUS): binary_sensor.binary_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),
            cv.Optional(CONF_RELAY_X13_X14_STATUS): binary_sensor.binary_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),
            cv.Optional(CONF_RELAY_X15_X16_STATUS): binary_sensor.binary_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),
            cv.Optional(CONF_RELAY_X17_X18_STATUS): binary_sensor.binary_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),
        }
    ),
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if CONF_HEATER_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_HEATER_STATUS])
        cg.add(var.set_heater_binary_sensor(sens))
    if CONF_LIGHT_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_LIGHT_STATUS])
        cg.add(var.set_light_binary_sensor(sens))
    if CONF_READY_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_READY_STATUS])
        cg.add(var.set_ready_binary_sensor(sens))
    if CONF_RELAY_X3_X4_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_RELAY_X3_X4_STATUS])
        cg.add(var.set_relay_x3_x4_binary_sensor(sens))
    if CONF_RELAY_X5_X6_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_RELAY_X5_X6_STATUS])
        cg.add(var.set_relay_x5_x6_binary_sensor(sens))
    if CONF_RELAY_X7_X8_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_RELAY_X7_X8_STATUS])
        cg.add(var.set_relay_x7_x8_binary_sensor(sens))
    if CONF_RELAY_X9_X10_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_RELAY_X9_X10_STATUS])
        cg.add(var.set_relay_x9_x10_binary_sensor(sens))
    if CONF_RELAY_X11_X12_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_RELAY_X11_X12_STATUS])
        cg.add(var.set_relay_x11_x12_binary_sensor(sens))
    if CONF_RELAY_X13_X14_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_RELAY_X13_X14_STATUS])
        cg.add(var.set_relay_x13_x14_binary_sensor(sens))
    if CONF_RELAY_X15_X16_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_RELAY_X15_X16_STATUS])
        cg.add(var.set_relay_x15_x16_binary_sensor(sens))
    if CONF_RELAY_X17_X18_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_RELAY_X17_X18_STATUS])
        cg.add(var.set_relay_x17_x18_binary_sensor(sens))
    sauna360 = await cg.get_variable(config[CONF_SAUNA360_ID])
    cg.add(sauna360.register_listener(var))