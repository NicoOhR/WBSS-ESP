Sensor driver based on the esp32-s3, currently aiming to support a Hall effect sensor and a linear potentiometer and spit back to the CAN bus.


## Testing
- [ ] ADC 
    - [x] with potentiometer
    - [ ] with chosen linpot
    - [ ] with external adc
        - [ ] bit-banging function to read from external adc
- [ ] CAN/TWAI
    - [x] Self test (loopback)
    - [ ] with transceiver
    - [ ] on the Pi
- [ ] PCNT
    - [x] with arbitrary digital signal
    - [ ] with wheel speed sensor
- [ ] timing
    - [x] benchmark at 100hz (9.5 kus left over)
    - [ ] try to go faster
- [ ] IMU
    - [x] read in loop back via I2C
        - [x] verify with DLHR-L10D
    - [ ] ~~read from IMU~~
        - The IMU has been cut for the time being

