import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_CONFIG

from .. import (
    sauna360_ns,
    SAUNA360Component,
    CONF_SAUNA360_ID,
)

CONF_RELAY_AUX0_MODE = "relay_aux0_mode"
CONF_RELAY_AUX1_MODE = "relay_aux1_mode"
CONF_RELAY_AUX2_MODE = "relay_aux2_mode"
CONF_AUX_SELECTS = [
    "Not in use",
    "On/Off",
    "Fragrance",
] 
CONF_EXTERNAL_SWITCH_MODE = "external_switching_mode"
CONF_EXTERNAL_SWITCH_SELECTS = [
    "On/Off",
    "Renew Bathtime",
] 
CONF_BATH_TYPE_PRIORITY = "bath_type_priority"
CONF_BATH_TYPE_PRIORITY_SELECTS = [
    "Automatic",
    "Temperature",
    "Humidity",
]

Sauna360RelayAux0Select = sauna360_ns.class_("Sauna360RelayAux0Select", cg.Component)
Sauna360RelayAux1Select = sauna360_ns.class_("Sauna360RelayAux1Select", cg.Component)
Sauna360RelayAux2Select = sauna360_ns.class_("Sauna360RelayAux2Select", cg.Component)
Sauna360ExternalSwitchSelect = sauna360_ns.class_("Sauna360ExternalSwitchSelect", cg.Component)
Sauna360BathTypePrioritySelect = sauna360_ns.class_("Sauna360BathTypePrioritySelect", cg.Component)

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(SAUNA360Component),
    cv.Optional(CONF_RELAY_AUX0_MODE): select.select_schema(
        Sauna360RelayAux0Select,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:cog-outline"
    ),
    cv.Optional(CONF_RELAY_AUX1_MODE): select.select_schema(
        Sauna360RelayAux1Select,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:cog-outline"
    ),
    cv.Optional(CONF_RELAY_AUX2_MODE): select.select_schema(
        Sauna360RelayAux2Select,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:cog-outline"
    ),
    cv.Optional(CONF_EXTERNAL_SWITCH_MODE): select.select_schema(
        Sauna360ExternalSwitchSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:cog-outline"
    ),
    cv.Optional(CONF_BATH_TYPE_PRIORITY): select.select_schema(
        Sauna360BathTypePrioritySelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:cog-outline"
    ),
}

async def to_code(config):
    SAUNA360_component = await cg.get_variable(config[CONF_SAUNA360_ID])
    if relay_aux0_mode_config := config.get(CONF_RELAY_AUX0_MODE):
        sel = await select.new_select(
            relay_aux0_mode_config,
            options=CONF_AUX_SELECTS,
        )
        await cg.register_parented(sel, config[CONF_SAUNA360_ID])
        cg.add(SAUNA360_component.set_aux0_relay_mode_select(sel))
    if relay_aux1_mode_config := config.get(CONF_RELAY_AUX1_MODE):
        sel = await select.new_select(
            relay_aux1_mode_config,
            options=CONF_AUX_SELECTS,
        )
        await cg.register_parented(sel, config[CONF_SAUNA360_ID])
        cg.add(SAUNA360_component.set_aux1_relay_mode_select(sel))
    if relay_aux2_mode_config := config.get(CONF_RELAY_AUX2_MODE):
        sel = await select.new_select(
            relay_aux2_mode_config,
            options=CONF_AUX_SELECTS,
        )
        await cg.register_parented(sel, config[CONF_SAUNA360_ID])
        cg.add(SAUNA360_component.set_aux2_relay_mode_select(sel))
    if external_switching_mode_config := config.get(CONF_EXTERNAL_SWITCH_MODE):
        sel = await select.new_select(
            external_switching_mode_config,
            options=CONF_EXTERNAL_SWITCH_SELECTS,
        )
        await cg.register_parented(sel, config[CONF_SAUNA360_ID])
        cg.add(SAUNA360_component.set_external_switching_mode_select(sel))
    if bath_type_priority_config := config.get(CONF_BATH_TYPE_PRIORITY):
        sel = await select.new_select(
            bath_type_priority_config,
            options=CONF_BATH_TYPE_PRIORITY_SELECTS,
        )
        await cg.register_parented(sel, config[CONF_SAUNA360_ID])
        cg.add(SAUNA360_component.set_bath_type_priority_select(sel))