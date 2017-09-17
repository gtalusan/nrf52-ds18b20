#include <BLEPeripheral.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

BLEPeripheral ble;

BLEService tempService("CCC0");
BLEFloatCharacteristic tempCharacteristic("CCC1", BLERead | BLENotify);
BLEDescriptor tempDescriptor("2901", "Temperature");

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

static unsigned long then = millis();

void setup(void)
{
	ble.setDeviceName("BLE/DS18B20");
	ble.setAdvertisedServiceUuid(tempService.uuid());
	ble.addAttribute(tempService);
	ble.addAttribute(tempCharacteristic);
	ble.addAttribute(tempDescriptor);

	sensors.begin();
	ble.begin();
}

void loop(void)
{
	ble.poll();
	unsigned long now = millis();
	if (now - then >= 3 * 1000) {
		sensors.requestTemperatures();
		tempCharacteristic.setValue(sensors.getTempCByIndex(0));
		then = now;
	}
}
