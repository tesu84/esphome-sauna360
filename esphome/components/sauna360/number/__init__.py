import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    UNIT_MINUTE,
    UNIT_SECOND,
    UNIT_EMPTY,
    UNIT_CELSIUS,
    UNIT_STEPS,
    ICON_TIMER,
    ICON_WATER_PERCENT,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY,

)
from .. import (
    sauna360_ns,
    SAUNA360Component,
    CONF_SAUNA360_ID,
)

SAUNA360BathTimeNumber = sauna360_ns.class_("SAUNA360BathTimeNumber", number.Number)
SAUNA360BathTemperatureNumber = sauna360_ns.class_("SAUNA360BathTemperatureNumber", number.Number)
SAUNA360StandbyTemperatureReductionNumber = sauna360_ns.class_("SAUNA360StandbyTemperatureReductionNumber", number.Number)
SAUNA360HumidityStepNumber = sauna360_ns.class_("SAUNA360HumidityStepNumber", number.Number)
SAUNA360MaxBathTemperatureNumber = sauna360_ns.class_("SAUNA360MaxBathTemperatureNumber", number.Number)
SAUNA360OverheatingPCBLimitNumber = sauna360_ns.class_("SAUNA360OverheatingPCBLimitNumber", number.Number)
SAUNA360ExternalSwitchRenewBathTimeNumber = sauna360_ns.class_("SAUNA360ExternalSwitchRenewBathTimeNumber", number.Number)
SAUNA360Aux0FragrancePumpNumber = sauna360_ns.class_("SAUNA360Aux0FragrancePumpNumber", number.Number)
SAUNA360Aux0FragranceStopNumber = sauna360_ns.class_("SAUNA360Aux0FragranceStopNumber", number.Number)
SAUNA360Aux1FragrancePumpNumber = sauna360_ns.class_("SAUNA360Aux1FragrancePumpNumber", number.Number)
SAUNA360Aux1FragranceStopNumber = sauna360_ns.class_("SAUNA360Aux1FragranceStopNumber", number.Number)
SAUNA360Aux2FragrancePumpNumber = sauna360_ns.class_("SAUNA360Aux2FragrancePumpNumber", number.Number)
SAUNA360Aux2FragranceStopNumber = sauna360_ns.class_("SAUNA360Aux2FragranceStopNumber", number.Number)

CONF_BATH_TIME = "bath_time"
CONF_BATH_TIME_DEFAULT = "bath_time_default"
CONF_BATH_TEMPERATURE = "bath_temperature"
CONF_BATH_TEMPERATURE_DEFAULT = "bath_temperature_default"
CONF_STANDBY_TEMPERATURE_REDUCTION ="standby_temperature_reduction"
CONF_STANDBY_TEMPERATURE_REDUCTION_DEFAULT ="standby_temperature_reduction_default"
CONF_HUMIDITY_STEP = "humidity_step"
CONF_HUMIDITY_STEP_DEFAULT = "humidity_step_default"
CONF_MAX_BATH_TEMPERATURE = "max_bath_temperature"
CONF_MAX_BATH_TEMPERATURE_DEFAULT = "max_bath_temperature_default"
CONF_OVERHEATING_PCB_LIMIT = "overheating_pcb_limit"
CONF_OVERHEATING_PCB_LIMIT_DEFAULT = "overheating_pcb_limit_default"
CONF_EXTERNAL_SWITCH_RENEW_BATH_TIME = "external_switch_renew_bathtime"
CONF_EXTERNAL_SWITCH_RENEW_BATH_TIME_DEFAULT = "external_switch_renew_bathtime_default"
CONF_AUX0_FRAGRANCE_PUMP = "aux0_fragrance_pump"
CONF_AUX0_FRAGRANCE_PUMP_DEFAULT = "aux0_fragrance_pump_default"
CONF_AUX0_FRAGRANCE_STOP = "aux0_fragrance_stop"
CONF_AUX0_FRAGRANCE_STOP_DEFAULT = "aux0_fragrance_stop_default"
CONF_AUX1_FRAGRANCE_PUMP = "aux1_fragrance_pump"
CONF_AUX1_FRAGRANCE_PUMP_DEFAULT = "aux1_fragrance_pump_default"
CONF_AUX1_FRAGRANCE_STOP = "aux1_fragrance_stop"
CONF_AUX1_FRAGRANCE_STOP_DEFAULT = "aux1_fragrance_stop_default"
CONF_AUX2_FRAGRANCE_PUMP = "aux2_fragrance_pump"
CONF_AUX2_FRAGRANCE_PUMP_DEFAULT = "aux2_fragrance_pump_default"
CONF_AUX2_FRAGRANCE_STOP = "aux2_fragrance_stop"
CONF_AUX2_FRAGRANCE_STOP_DEFAULT = "aux2_fragrance_stop_default"


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(SAUNA360Component),
        cv.Optional(CONF_BATH_TIME): number.number_schema(
            SAUNA360BathTimeNumber,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_MINUTE,
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
            icon="mdi:thermometer-lines",
        ).extend(
            {
                cv.Optional(CONF_BATH_TEMPERATURE_DEFAULT): cv.float_range(min=40, max=110),
            }
        ),
        cv.Optional(CONF_STANDBY_TEMPERATURE_REDUCTION): number.number_schema(
            SAUNA360StandbyTemperatureReductionNumber,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer-chevron-down",
        ).extend(
            {
                cv.Optional(CONF_STANDBY_TEMPERATURE_REDUCTION_DEFAULT): cv.float_range(min=0, max=30),
            }
        ),
        cv.Optional(CONF_HUMIDITY_STEP): number.number_schema(
            SAUNA360HumidityStepNumber,
            device_class=DEVICE_CLASS_HUMIDITY,
            unit_of_measurement=UNIT_STEPS,
            icon=ICON_WATER_PERCENT,
        ).extend(
            {
                cv.Optional(CONF_HUMIDITY_STEP_DEFAULT): cv.float_range(min=0, max=10),
            }
        ),
        cv.Optional(CONF_MAX_BATH_TEMPERATURE): number.number_schema(
            SAUNA360MaxBathTemperatureNumber,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer-lines",
        ).extend(
            {
                cv.Optional(CONF_MAX_BATH_TEMPERATURE_DEFAULT): cv.float_range(min=40, max=110),
            }
        ),
        cv.Optional(CONF_OVERHEATING_PCB_LIMIT): number.number_schema(
            SAUNA360OverheatingPCBLimitNumber,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer-alert",
        ).extend(
            {
                cv.Optional(CONF_OVERHEATING_PCB_LIMIT_DEFAULT): cv.float_range(min=70, max=90),
            }
        ),
        cv.Optional(CONF_EXTERNAL_SWITCH_RENEW_BATH_TIME): number.number_schema(
            SAUNA360ExternalSwitchRenewBathTimeNumber,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_MINUTE,
            icon=ICON_TIMER,
        ).extend(
            {
                cv.Optional(CONF_EXTERNAL_SWITCH_RENEW_BATH_TIME_DEFAULT): cv.float_range(min=1, max=360),
            }
        ),
        cv.Optional(CONF_AUX0_FRAGRANCE_PUMP): number.number_schema(
            SAUNA360Aux0FragrancePumpNumber,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_TIMER,
        ).extend(
            {
                cv.Optional(CONF_AUX0_FRAGRANCE_PUMP_DEFAULT): cv.float_range(min=1, max=1200),
            }
        ),
        cv.Optional(CONF_AUX0_FRAGRANCE_STOP): number.number_schema(
            SAUNA360Aux0FragranceStopNumber,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            icon=ICON_TIMER,
        ).extend(
            {
                cv.Optional(CONF_AUX0_FRAGRANCE_STOP_DEFAULT): cv.float_range(min=1, max=7200),
            }
        ),
        cv.Optional(CONF_AUX1_FRAGRANCE_PUMP): number.number_schema(
            SAUNA360Aux1FragrancePumpNumber,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_TIMER,
        ).extend(
            {
                cv.Optional(CONF_AUX1_FRAGRANCE_PUMP_DEFAULT): cv.float_range(min=1, max=1200),
            }
        ),
        cv.Optional(CONF_AUX1_FRAGRANCE_STOP): number.number_schema(
            SAUNA360Aux1FragranceStopNumber,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            icon=ICON_TIMER,
        ).extend(
            {
                cv.Optional(CONF_AUX1_FRAGRANCE_STOP_DEFAULT): cv.float_range(min=1, max=7200),
            }
        ),
        cv.Optional(CONF_AUX2_FRAGRANCE_PUMP): number.number_schema(
            SAUNA360Aux2FragrancePumpNumber,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_TIMER,
        ).extend(
            {
                cv.Optional(CONF_AUX2_FRAGRANCE_PUMP_DEFAULT): cv.float_range(min=1, max=1200),
            }
        ),
        cv.Optional(CONF_AUX2_FRAGRANCE_STOP): number.number_schema(
            SAUNA360Aux2FragranceStopNumber,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            icon=ICON_TIMER,
        ).extend(
            {
                cv.Optional(CONF_AUX2_FRAGRANCE_STOP_DEFAULT): cv.float_range(min=1, max=7200),
            }
        ),
    }
)

async def to_code(config):
    sauna360_component = await cg.get_variable(config[CONF_SAUNA360_ID])
    if bath_time := config.get(CONF_BATH_TIME):
      n = await number.new_number(
        bath_time, min_value=1, max_value=360, step=1,
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
    if standby_temperature_reduction := config.get(CONF_STANDBY_TEMPERATURE_REDUCTION):
      n = await number.new_number(
        standby_temperature_reduction, min_value=0, max_value=30, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_standby_temperature_reduction_number(n))
      if CONF_STANDBY_TEMPERATURE_REDUCTION_DEFAULT in standby_temperature_reduction:
        cg.add(sauna360_component.set_standby_temperature_reduction_default_value(standby_temperature_reduction[CONF_STANDBY_TEMPERATURE_REDUCTION_DEFAULT]))
    if humidity_step := config.get(CONF_HUMIDITY_STEP):
      n = await number.new_number(
        humidity_step, min_value=0, max_value=10, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_humidity_step_number(n))
      if CONF_HUMIDITY_STEP_DEFAULT in humidity_step:
        cg.add(sauna360_component.set_humidity_step_default_value(humidity_step[CONF_HUMIDITY_STEP_DEFAULT]))
    if max_bath_temperature := config.get(CONF_MAX_BATH_TEMPERATURE):
      n = await number.new_number(
        max_bath_temperature, min_value=40, max_value=110, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_max_bath_temperature_number(n))
      if CONF_MAX_BATH_TEMPERATURE_DEFAULT in max_bath_temperature:
        cg.add(sauna360_component.set_max_bath_temperature_default_value(max_bath_temperature[CONF_MAX_BATH_TEMPERATURE_DEFAULT]))
    if overheating_pcb_limit := config.get(CONF_OVERHEATING_PCB_LIMIT):
      n = await number.new_number(
        overheating_pcb_limit, min_value=70, max_value=90, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_overheating_pcb_limit_number(n))
      if CONF_OVERHEATING_PCB_LIMIT_DEFAULT in overheating_pcb_limit:
        cg.add(sauna360_component.set_overheating_pcb_limit_default_value(overheating_pcb_limit[CONF_OVERHEATING_PCB_LIMIT_DEFAULT]))
    if external_switch_renew_bathtime := config.get(CONF_EXTERNAL_SWITCH_RENEW_BATH_TIME):
      n = await number.new_number(
        external_switch_renew_bathtime, min_value=1, max_value=360, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_external_switch_renew_bathtime_number(n))
      if CONF_EXTERNAL_SWITCH_RENEW_BATH_TIME_DEFAULT in external_switch_renew_bathtime:
        cg.add(sauna360_component.set_external_switch_renew_bathtime_default_value(external_switch_renew_bathtime[CONF_EXTERNAL_SWITCH_RENEW_BATH_TIME_DEFAULT]))
    if aux0_fragrance_pump := config.get(CONF_AUX0_FRAGRANCE_PUMP):
      n = await number.new_number(
        aux0_fragrance_pump, min_value=1, max_value=1200, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_aux0_fragrance_pump_number(n))
      if CONF_AUX0_FRAGRANCE_PUMP_DEFAULT in aux0_fragrance_pump:
        cg.add(sauna360_component.set_aux0_fragrance_pump_default_value(aux0_fragrance_pump[CONF_AUX0_FRAGRANCE_PUMP_DEFAULT]))
    if aux0_fragrance_stop := config.get(CONF_AUX0_FRAGRANCE_STOP):
      n = await number.new_number(
        aux0_fragrance_stop, min_value=1, max_value=7200, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_aux0_fragrance_stop_number(n))
      if CONF_AUX0_FRAGRANCE_STOP_DEFAULT in aux0_fragrance_stop:
        cg.add(sauna360_component.set_aux0_fragrance_stop_default_value(aux0_fragrance_stop[CONF_AUX0_FRAGRANCE_STOP_DEFAULT]))
    if aux1_fragrance_pump := config.get(CONF_AUX1_FRAGRANCE_PUMP):
      n = await number.new_number(
        aux1_fragrance_pump, min_value=1, max_value=1200, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_aux1_fragrance_pump_number(n))
      if CONF_AUX1_FRAGRANCE_PUMP_DEFAULT in aux1_fragrance_pump:
        cg.add(sauna360_component.set_aux1_fragrance_pump_default_value(aux1_fragrance_pump[CONF_AUX1_FRAGRANCE_PUMP_DEFAULT]))
    if aux1_fragrance_stop := config.get(CONF_AUX1_FRAGRANCE_STOP):
      n = await number.new_number(
        aux1_fragrance_stop, min_value=1, max_value=7200, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_aux1_fragrance_stop_number(n))
      if CONF_AUX1_FRAGRANCE_STOP_DEFAULT in aux1_fragrance_stop:
        cg.add(sauna360_component.set_aux1_fragrance_stop_default_value(aux1_fragrance_stop[CONF_AUX1_FRAGRANCE_STOP_DEFAULT]))
    if aux2_fragrance_pump := config.get(CONF_AUX2_FRAGRANCE_PUMP):
      n = await number.new_number(
        aux2_fragrance_pump, min_value=1, max_value=1200, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_aux2_fragrance_pump_number(n))
      if CONF_AUX2_FRAGRANCE_PUMP_DEFAULT in aux2_fragrance_pump:
        cg.add(sauna360_component.set_aux2_fragrance_pump_default_value(aux2_fragrance_pump[CONF_AUX2_FRAGRANCE_PUMP_DEFAULT]))
    if aux2_fragrance_stop := config.get(CONF_AUX2_FRAGRANCE_STOP):
      n = await number.new_number(
        aux2_fragrance_stop, min_value=1, max_value=7200, step=1,
      )
      await cg.register_parented(n, sauna360_component)
      cg.add(sauna360_component.set_aux2_fragrance_stop_number(n))
      if CONF_AUX2_FRAGRANCE_STOP_DEFAULT in aux2_fragrance_stop:
        cg.add(sauna360_component.set_aux2_fragrance_stop_default_value(aux2_fragrance_stop[CONF_AUX2_FRAGRANCE_STOP_DEFAULT]))