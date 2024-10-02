## Description

This component is stripped down of espressif example [network/sta2eth](https://github.com/espressif/esp-idf/tree/master/examples/network/sta2eth) to provide webserver over USB functionality.

The mine goal was to provide option to run webserver on standalone esp32-P4 which does not have radio, but it can be used on S2 which does not have BLE for wifi provisioning or just on any esp32 with hardware USB for whatever reason we want.

It is early version (more like PoC), so APIs may change or evolve.

## Usage
This is the minimal code required to start running TCP/IP over USB (when passing `NULL` optional params will be used):
```
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());

    usb_ip_init_defaul_config();
```

## TODO
- add options in Kconfig to configure
    - OTA over USB
    - create http server
- add optional http server module
- add optional OTA module

## Future features
- add usb to wifi bridge
- add usb to eth bridge
- add usb to other TCP/IP stacks (thread, zigbee)

## License

MIT License

Copyright (c) 2024 esp32-open-source

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
