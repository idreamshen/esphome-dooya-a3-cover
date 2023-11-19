import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover, uart
from esphome.const import CONF_ID, CONF_UART_ID

DEPENDENCIES = ['uart']
AUTO_LOAD = ['cover']

dooya_a3_cover_ns = cg.esphome_ns.namespace('dooya_a3_cover')
DooyaA3Cover = dooya_a3_cover_ns.class_('DooyaA3Cover', cg.PollingComponent, cover.Cover, uart.UARTDevice)

CONFIG_SCHEMA = cover.COVER_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(DooyaA3Cover),
    cv.Required("id_low"): cv.hex_int,
    cv.Required("id_high"): cv.hex_int,
}).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    u = yield cg.get_variable(config[CONF_UART_ID])
    var = cg.new_Pvariable(config[CONF_ID], u)
    cg.add(var.set_id_low(config["id_low"]))
    cg.add(var.set_id_high(config["id_high"]))
    yield cg.register_component(var, config)
    yield cover.register_cover(var, config)
    yield uart.register_uart_device(var, config)
