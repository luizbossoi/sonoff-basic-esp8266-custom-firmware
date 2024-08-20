# Sonoff Basic - Custom Firmware

This custom firmware allow you flash your Sonoff Basic to work HTTP based.
With this custom firmware you don't need to hard code a static WiFi connection, you can configure your Sonoff in the first boot time.
After flashing, you can select your desired home WiFi and make HTTP requests through the browser or using JSON calls. A Serial Interface is implemented for debugging purposes.

It's easier to integrate with your current home automation or with your custom mobile application.


**FIRST OF ALL**
Unplug your Sonoff from the wall, remove all AC connections, you DON'T NEED to plug it anywhere. If you ignore this step, you can fry your computer or take a shock.

# What you need
1. Basic soldering iron
2. Male socket strip (PCB male pin header bar)
3. FTDI FT232RL USB/Serial
4. Jumper Female<>Female Cables
5. Sonoff Basic

# Softwares
1. Arduino IDE https://www.arduino.cc/en/main/software

# Soldering header bars
Sonoff already has in its PCB holes, you just need to fit the four header's bar into Sonoff and solder into.

![alt text](https://raw.githubusercontent.com/luizbossoi/sonoff-basic-esp8266-custom-firmware/master/images/sonoff+pins.png)

# Attach FTDI converter on Sonoff PCB.
Grab your female jumper cables and connect FTDI Converter on Sonoff PCB.

**TAKE CARE** Make sure your FTDI voltage is set to 3V3 volts instead of 5V. In this example, there is a jumper on FTDI converter where you can select the voltage by connecting the middle pin with the 3V3 pin.

You have to connect in the following order:

	Sonoff	<>	FTDI
	3V3	<>	VCC
	RX	<>	TX
	TX	<>	RX
	GND	<>	GND
	
![alt text](https://raw.githubusercontent.com/luizbossoi/sonoff-basic-esp8266-custom-firmware/master/images/connections.png)

![alt text](https://raw.githubusercontent.com/luizbossoi/sonoff-basic-esp8266-custom-firmware/master/images/sonoff+ftdi.png)

# Prepare Arduino software
After installed, open Arduino IDE software and follow these steps.
1. Header menu "Tools",
2. Boards
3. Manage Boards
4. Look for "esp8266"
5. Install latest version of ESP8266 by ESP8266 Community
6. In Tools -> Board -> "ESP8266" -> Select "Generic ESP8266 Module"

![alt text](https://raw.githubusercontent.com/luizbossoi/sonoff-basic-esp8266-custom-firmware/master/images/board-esp8266.png)

Next step, you need to add WiFiManager library,
1. Header menu "Tools",
2. Manage Libraries
3. Find for "WiFiManager"
4. Install "WiFiManager" by tzapu, on latest version (version 0.14.0 used in this source);

![alt text](https://raw.githubusercontent.com/luizbossoi/sonoff-basic-esp8266-custom-firmware/master/images/library-wifimanager.png)

Done.

# Start Sonoff on flashing mode.
Now you have everything installed and wired, you need to start Sonoff on Flashing mode.

Plug your USB cable on FTDI, press and hold sonoff button and insert the other side of your USB cable on your computer.
Wait 4 seconds and release the button. Now you might be in flashing mode.

# Flashing the new Firmware
Now your Sonoff is in flashing mode, open 'sonoff-custom-firmware.ino' on Arduino IDE.
Make sure your are using the correct port on Arduino IDE ( Menu Tools > Port > Your communication port )
After that, you can now press "Upload" button on Arduino IDE and wait the compilation and upload to Sonoff.

You will see something like this in the Console window:

	Running stub...
	Stub running...
	Configuring flash size...
	Auto-detected Flash size: 1MB
	Flash params set to 0x0320
	Compressed 313872 bytes to 223358...

	Writing at 0x00000000... (7 %)
	Writing at 0x00004000... (14 %)
	(..............)
	Writing at 0x00030000... (92 %)
	Writing at 0x00034000... (100 %)
	Wrote 313872 bytes (223358 compressed) at 0x00000000 in 19.8 seconds (effective 126.6 kbit/s)...
	Hash of data verified.

	Leaving...
	Hard resetting via RTS pin...

At this momment, your Sonoff has the custom firmware installed but you need to reboot it manually to boot from the new firmware.
Just unplug and plug it again on your computer, you don't need to start it again in flashing mode.

# And now?
Now, you have a custom Sonoff firmware working! 
Get your mobile phone (it's easier here) and list your nearby wifi networks

You will probably see (if everything is OK!) something like "Sonoff-00000000000' wifi network. Take note of this name, it's important!
This different network is your setup network, you have to use this network to setup our home network.

![alt text](https://raw.githubusercontent.com/luizbossoi/sonoff-basic-esp8266-custom-firmware/master/images/wifi1.png)

Request to connect and you'll see 'Internet is unavailable', that's OK, just ignore.
After connected, click on it again and click in "Manage Router".

![alt text](https://raw.githubusercontent.com/luizbossoi/sonoff-basic-esp8266-custom-firmware/master/images/wifi2.png)

Your default browser will open with the configuration page, just follow the steps and it will connect to your local wifi network.
If everything is OK, you will not see the "Sonoff-00000000" in your nearby wifi networks again.

![alt text](https://raw.githubusercontent.com/luizbossoi/sonoff-basic-esp8266-custom-firmware/master/images/wifi3.png)

All ok?!

# Check everything
Ok, you did the upload, you could see the "Sonoff-00000000" network in your nearby WiFi list, you did the initial WiFi configuration and you can't see the setup wifi network again.
Sounds good, you problably did everything right!

Now, in your computer or mobile phone, open your browser and try to access your new Sonoff Firmware.
Do you remember it was necessary to take note of your Sonoff ID? Now you have to use it again.

Type: http://Sonoff-0000000000/

Can you see buttons ON - OFF / RESET?
Using this buttons you can start / stop sonoff relay and "RESET" you can reset all network configurations, back to stock (you will be able to see the setup network again).

![alt text](https://raw.githubusercontent.com/luizbossoi/sonoff-basic-esp8266-custom-firmware/master/images/sonoff-website.png)

You *CAN'T* access the URL above and can't see these buttons?
Well, you're probably using another DNS in your computer (different from your router's default DNS) and in that case you have to identify which IP was set to your Sonoff relay.
There are several ways to do that, the easiest is by logging into your router admin panel and listing all devices connected to your router.

Or if you have an Android Phone, you can install this app and get the list of all connected devices in your WiFi
https://play.google.com/store/apps/details?id=us.amzwaru.whousesmywifi

# Additional Features

## JSON

You can make JSON requests to get relay status and set ON/OFF, the following requests can be done:

	/json - Get statuses
	/json/on - Set the relay ON
	/json/off - Set the relay OFF

All responses are in JSON Format like:
	{"device":"0000000000", "status":"on"}

## Default relay status
In a case of power loss, this Firmware will not remember the last relay state and by default will set the relay as OFF.
You can change this behavior by changing variable "DEFAULT_RELAYSTATUS" from LOW to HIGH.

## Manual network reset
If you want, you can reset the network by pressing Sonoff push button for 5 seconds, it will reset your network configuration and restore WiFi configuration network.

## Automation
With this firmware, you are free to automate custom HTTP calls, for example, you can schedule to turn on or turn off your Sonoff relay when you desire.
As this firmware allows JSON HTTP requests, you can create linux schedules, for example, with CURL and automate your place.

A basic crontab example to turn on the light at midnight and turn off at 6 AM would be:

	0 0 * * *	curl --silent http://Sonoff-0000000000/json/on &>/dev/null
	0 6 * * *	curl --silent http://Sonoff-0000000000/json/off &>/dev/null

RAW example of curl request to turn the relay ON:
	curl http://Sonoff-0000000000/json/on

![alt text](https://raw.githubusercontent.com/luizbossoi/sonoff-basic-esp8266-custom-firmware/master/images/ssh-curl.png)

## Serial Interface
If the Sonoff is attached to the FTDI converter to your computer, you could open the Arduino IDEs Serial Monitor. It will log WiFi status changes and incoming calls of on and off.
You could send messages to the board.

  * 1 → switch the relay on
  * 2 → switch it off, 
  * r → reconnects the WiFi with the current credentials  
  * w → returns the current WiFi.status()
  * default → returns a command overview "on: 1, off: 2, reconnectWifi: r, WiFiStatus: w"

## WiFi reconnect
The WiFi Class provides a method: WiFi.setAutoReconnect(true); but that does not work as expected on my device. I added a keepWifiAlive() which is called in the loop and checks the current WiFi.status, compares it with the last and reconnects if it is not connected any more. If the WiFi Host is down for a while, the Sonoff will try to reconnect every 13s.

# FAQ

### 1. Does it work with another Sonoff version?
Not tested, just with Sonoff Basic.

### 2. Can I fork this repository or make changes in this script
Feel free.

### 3. What happens if I push the Sonoff button?
By default, if you press the button you can toggle the relay.
If you press for more than 5 seconds, you will reset your Sonoff network configurations to default, then the WiFi network configuration "Sonoff-0000000000" will be available again.

### 4. Everything was working fine, I did not reset the configuration and now I'm seeing "Sonoff-00000000" network again , why?
If you don't request a network reset, this might be related to your home WiFi settings. Maybe it changed or the signal is weak?
When this firmware try to contact your home WiFi and it cannot be reached (due to wrong password, bad signal, interference, etc), it enables the initial network configuration back again, so you can reconfigure your Sonoff remotely, without the need to have it on hands.
This is good, if you have this equipment inside your roof, for example.

Anyways, if it's not the case, for example you just did a reboot on your router, just hold on, the default setup network will disappear and everything will be back as normal.
You can change this "timeout" setting on wifiManager.setConfigPortalTimeout(180) from 180 (3 minutes) to another value, but I don't think it's necessary.... up to you.

That's it.

