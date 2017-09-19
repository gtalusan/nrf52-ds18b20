#include <BLEPeripheral.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <nrf_nvic.h>

#define ONE_WIRE_BUS 4

BLEPeripheral ble;

BLEService tempService("CCC0");
BLEFloatCharacteristic tempCharacteristic("CCC1", BLERead/* | BLENotify*/);
BLEDescriptor tempDescriptor("2901", "Temperature");

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

static unsigned long then = millis();
static bool _connected = false;

void setup(void)
{
	ble.setDeviceName("BLE/DS18B20");
	ble.setAdvertisedServiceUuid(tempService.uuid());
	ble.addAttribute(tempService);
	ble.addAttribute(tempCharacteristic);
	ble.addAttribute(tempDescriptor);
	ble.setEventHandler(BLEConnected, [=] (BLECentral &central) {
		_connected = true;
	});
	ble.setEventHandler(BLEDisconnected, [=] (BLECentral &central) {
		_connected = false;
		sd_nvic_SystemReset();
	});

	sensors.begin();
	ble.begin();
}

void loop(void)
{
	ble.poll();
	unsigned long now = millis();
	if (_connected) {
		if (now - then >= 3 * 1000) {
			sensors.requestTemperatures();
			tempCharacteristic.setValue(sensors.getTempCByIndex(0));
			then = now;
		}
	}
	else {		
		sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
		sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
		sd_app_evt_wait();
	}
}
