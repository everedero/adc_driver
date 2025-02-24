# Test application for ADC driver

## Download repository
```shell
west init -m https://github.com/everedero/adc_driver --mr main my-workspace
cd my-workspace
west update
```

## Build and flash
```shell
BOARD="arduino_due"
west build -b $BOARD blinky
```

Once you have built the application, run the following command to flash it:

```shell
west flash
```

It might be needed to press "erase" for 1s and "reset" for 1s.
