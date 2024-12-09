import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_SWITCH,
    ICON_LIGHTBULB,
    ENTITY_CATEGORY_CONFIG,
)
from .. import (
    sauna360_ns,
    SAUNA360Component,
    CONF_SAUNA360_ID,
)

SAUNA360LightRelaySwitch = sauna360_ns.class_("SAUNA360LightRelaySwitch", switch.Switch, cg.Component)
SAUNA360Aux0RelaySwitch = sauna360_ns.class_("SAUNA360Aux0RelaySwitch", switch.Switch, cg.Component)
SAUNA360Aux1RelaySwitch = sauna360_ns.class_("SAUNA360Aux1RelaySwitch", switch.Switch, cg.Component)
SAUNA360Aux2RelaySwitch = sauna360_ns.class_("SAUNA360Aux2RelaySwitch", switch.Switch, cg.Component)
SAUNA360StandbyEnableSwitch = sauna360_ns.class_("SAUNA360StandbyEnableSwitch", switch.Switch, cg.Component)
SAUNA360ActivateTimeLimitSwitch = sauna360_ns.class_("SAUNA360ActivateTimeLimitSwitch", switch.Switch, cg.Component)

CONF_LIGHT_RELAY = "light_relay"
CONF_AUX0_RELAY = "aux0_relay"
CONF_AUX1_RELAY = "aux1_relay"
CONF_AUX2_RELAY = "aux2_relay"
CONF_STANBY_ENABLE = "standby_enable"
CONF_ACTIVATE_TIME_LIMIT = "activate_time_limit"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(SAUNA360Component),
    cv.Optional(CONF_LIGHT_RELAY): switch.switch_schema(
        SAUNA360LightRelaySwitch,
        device_class=DEVICE_CLASS_SWITCH,
        icon=ICON_LIGHTBULB,
    ),
    cv.Optional(CONF_AUX0_RELAY): switch.switch_schema(
        SAUNA360Aux0RelaySwitch,
        device_class=DEVICE_CLASS_SWITCH,
        icon="mdi:electric-switch",
    ),
    cv.Optional(CONF_AUX1_RELAY): switch.switch_schema(
        SAUNA360Aux1RelaySwitch,
        device_class=DEVICE_CLASS_SWITCH,
        icon="mdi:electric-switch",
    ),
    cv.Optional(CONF_AUX2_RELAY): switch.switch_schema(
        SAUNA360Aux2RelaySwitch,
        device_class=DEVICE_CLASS_SWITCH,
        icon="mdi:electric-switch",
    ),
    cv.Optional(CONF_STANBY_ENABLE): switch.switch_schema(
        SAUNA360StandbyEnableSwitch,
        device_class=DEVICE_CLASS_SWITCH,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:cog-outline",
    ),
    cv.Optional(CONF_ACTIVATE_TIME_LIMIT): switch.switch_schema(
        SAUNA360ActivateTimeLimitSwitch,
        device_class=DEVICE_CLASS_SWITCH,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:cog-outline",
    ),
}

async def to_code(config):
    sauna360_component = await cg.get_variable(config[CONF_SAUNA360_ID])
    if light_relay := config.get(CONF_LIGHT_RELAY):
        s = await switch.new_switch(light_relay)
        await cg.register_parented(s, config[CONF_SAUNA360_ID])
        cg.add(sauna360_component.set_light_relay_switch(s))
    if aux0_relay := config.get(CONF_AUX0_RELAY):
        s = await switch.new_switch(aux0_relay)
        await cg.register_parented(s, config[CONF_SAUNA360_ID])
        cg.add(sauna360_component.set_aux0_relay_switch(s))
    if aux1_relay := config.get(CONF_AUX1_RELAY):
        s = await switch.new_switch(aux1_relay)
        await cg.register_parented(s, config[CONF_SAUNA360_ID])
        cg.add(sauna360_component.set_aux1_relay_switch(s))
    if aux2_relay := config.get(CONF_AUX2_RELAY):
        s = await switch.new_switch(aux2_relay)
        await cg.register_parented(s, config[CONF_SAUNA360_ID])
        cg.add(sauna360_component.set_aux2_relay_switch(s))
    if standby_enable := config.get(CONF_STANBY_ENABLE):
        s = await switch.new_switch(standby_enable)
        await cg.register_parented(s, config[CONF_SAUNA360_ID])
        cg.add(sauna360_component.set_standby_enable_switch(s))
    if activate_time_limit := config.get(CONF_ACTIVATE_TIME_LIMIT):
        s = await switch.new_switch(activate_time_limit)
        await cg.register_parented(s, config[CONF_SAUNA360_ID])
        cg.add(sauna360_component.set_activate_time_limit_switch(s))