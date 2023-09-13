function get_device_name(device, value) {
	return device + "_" + value;
}

function Decoder(request) {

	var data = JSON.parse(request.body);

	var device = data.device;
	console.log(device);
	var decoded = {};

	decoded.sensor = data.body.node_id;

	if (typeof data.body.voltage_1 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "voltage")] = data.body.voltage_1;
	}
	if (typeof data.body.humidity_2 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "humidity")] = data.body.humidity_2;
	}
	if (typeof data.body.temperature_3 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "temperature")] = data.body.temperature_3;
	}
	if (typeof data.body.barometer_4 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "pressure")] = data.body.barometer_4;
	}
	if (typeof data.body.illuminance_5 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "illuminance")] = data.body.illuminance_5;
	}
	if (typeof data.body.humidity_6 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "humidity")] = data.body.humidity_6;
	}
	if (typeof data.body.temperature_7 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "temperature")] = data.body.temperature_7;
	}
	if (typeof data.body.barometer_8 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "pressure")] = data.body.barometer_8;
	}
	if (typeof data.body.analog_9 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "voc")] = data.body.analog_9;
	}
	if (typeof data.body.gps_10 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "location")] = data.body.gps_10;
	}
	if (typeof data.body.gps_10 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "location")] = data.body.gps_10;
	}
	if (typeof data.body.temperature_11 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "soil_temp")] = data.body.temperature_11;
	}
	if (typeof data.body.humidity_12 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "soil_humid")] = data.body.humidity_12;
	}
	if (typeof data.body.illuminance_15 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "illuminance")] = data.body.illuminance_15;
	}
	if (typeof data.body.voc_16 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "voc")] = data.body.voc_16;
	}
	if (typeof data.body.analog_in_17 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "mq2_gas")] = data.body.analog_in_17;
	}
	if (typeof data.body.percentage_18 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "mq2_gas_perc")] = data.body.percentage_18;
	}
	if (typeof data.body.analog_in_19 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "co2_gas")] = data.body.analog_in_19;
	}
	if (typeof data.body.percentage_20 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "co2_gas_perc")] = data.body.percentage_20;
	}
	if (typeof data.body.analog_in_21 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "mq3_gas")] = data.body.analog_in_21;
	}
	if (typeof data.body.percentage_22 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "mq3_gas_perc")] = data.body.percentage_22;
	}
	if (typeof data.body.analog_in_23 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "tof_distance")] = data.body.analog_in_23;
	}
	if (typeof data.body.presence_24 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "tof_valid")] = data.body.presence_24;
	}
	if (typeof data.body.gyrometer_25 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "gyro")] = data.body.gyrometer_25;
	}
	if (typeof data.body.digital_in_26 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "gesture")] = data.body.digital_in_26;
	}
	if (typeof data.body.analog_in_27 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "uvi")] = data.body.analog_in_27;
	}
	if (typeof data.body.illuminance_28 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "uvs")] = data.body.illuminance_28;
	}
	if (typeof data.body.analog_29 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "ina219_current")] = data.body.analog_29;
	}
	if (typeof data.body.analog_30 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "ina219_voltage")] = data.body.analog_30;
	}
	if (typeof data.body.analog_31 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "ina219_voltage")] = data.body.analog_31;
	}
	if (typeof data.body.presence_32 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "tp_left")] = data.body.presence_32;
	}
	if (typeof data.body.presence_33 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "tp_center")] = data.body.presence_33;
	}
	if (typeof data.body.presence_34 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "tp_right")] = data.body.presence_34;
	}
	if (typeof data.body.concentration_35 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "co2")] = data.body.concentration_35;
	}
	if (typeof data.body.temperature_39 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "mlx_temperature")] = data.body.temperature_39;
	}
	if (typeof data.body.voc_40 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "pm1_0")] = data.body.voc_40;
	}
	if (typeof data.body.voc_41 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "pm2_5")] = data.body.voc_41;
	}
	if (typeof data.body.voc_42 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "pm10")] = data.body.voc_42;
	}
	if (typeof data.body.presence_43 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "earthqake_alert")] = data.body.presence_43;
	}
	if (typeof data.body.analog_44 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "earthquake_si")] = data.body.analog_44;
	}
	if (typeof data.body.analog_45 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "earthquake_pga")] = data.body.analog_45;
	}
	if (typeof data.body.presence_46 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "earthquake_shutoff")] = data.body.presence_46;
	}
	if (typeof data.body.presence_47 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "earthquake_collapse")] = data.body.presence_47;
	}
	if (typeof data.body.presence_48 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "switch")] = data.body.presence_48;
	}
	if (typeof data.body.wind_speed_49 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "wind_speed")] = data.body.wind_speed_49;
	}
	if (typeof data.body.wind_direction_50 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "wind_direction")] = data.body.wind_direction_50;
	}
	if (typeof data.body.analog_61 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "water_level")] = data.body.analog_61;
	}
	if (typeof data.body.presence_62 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "low_level_alert")] = data.body.presence_62;
	}
	if (typeof data.body.presence_63 != 'undefined') {
		decoded[get_device_name(data.body.node_id, "overflow_alert")] = data.body.presence_63;
	}

	if (("tower_lat" in data) && ("tower_lon" in data)) {
		decoded.tower_location = "(" + data.tower_lat + "," + data.tower_lon + ")";
	}
	if (("where_lat" in data) && ("where_lon" in data)) {
		decoded.device_location = "(" + data.where_lat + "," + data.where_lon + ")";
	}

	decoded.rssi = data.rssi;
	decoded.bars = data.bars;
	decoded.temp = data.temp;
	decoded.orientation = data.orientation;
	decoded.card_temperature = data.body.temperature;

	// Array where we store the fields that are being sent to Datacake
	var datacakeFields = []

	// take each field from decodedElsysFields and convert them to Datacake format
	for (var key in decoded) {
		if (decoded.hasOwnProperty(key)) {
			datacakeFields.push({ field: key.toUpperCase(), value: decoded[key], device: device })
		}
	}

	// forward data to Datacake
	return datacakeFields;

}