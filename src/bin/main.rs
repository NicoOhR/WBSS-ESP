#![no_std]
#![no_main]
use core::cmp::min;
use core::prelude::*;
use esp_backtrace as _;
use esp_hal::clock::CpuClock;
use esp_hal::delay;
use esp_hal::gpio::{AnyPin, GpioPin, Input, InputPin, Io, Level, Output, OutputPin, Pin, Pull};
use esp_hal::peripheral::{Peripheral, PeripheralRef};
use esp_hal::peripherals::Peripherals;
use esp_hal::rtc_cntl::Rtc;
use esp_hal::time::{Duration, ExtU64};
use esp_hal::{
    analog::adc::{Adc, AdcConfig, Attenuation},
    i2c::master::*,
    main,
    pcnt::{channel, Pcnt},
    time,
    timer::*,
    twai::{self, filter::SingleStandardFilter, EspTwaiFrame, StandardId, TwaiMode},
};
use esp_hal::{dma_buffers, peripherals};
use esp_println::println;
use timg::TimerGroup;

const CAN_BAUDRATE: twai::BaudRate = twai::BaudRate::B250K;

struct spi_bitbang<'a> {
    sclk: Output<'a>,
    miso: Input<'a>,
    cs: Output<'a>,
}

impl spi_bitbang<'_> {
    fn new<P1, P2, P3>(sclk: P1, miso: P2, cs: P3) -> Self
    where
        P1: Into<AnyPin>,
        P2: Into<AnyPin>,
        P3: Into<AnyPin>,
    {
        Self {
            sclk: Output::new(sclk.into(), Level::High),
            miso: Input::new(miso.into(), Pull::Down),
            cs: Output::new(cs.into(), Level::High),
        }
    }
    fn get_adc(&mut self) -> u16 {
        let mut ad: u16 = 0b0;
        self.cs.set_low();
        for i in 0..6 {
            self.sclk.set_high();
            self.sclk.set_low();
        }

        for i in 0..16 {
            ad <<= 1;
            if self.miso.is_high() {
                ad |= 1;
            }
            self.sclk.set_high();
            self.sclk.set_low();
        }

        self.cs.set_high();
        ad
    }
}

#[main]
fn main() -> ! {
    #[allow(unused)]
    let mut config = esp_hal::Config::default();

    config.cpu_clock = CpuClock::max();

    let peripherals = esp_hal::init(config);
    let io = Io::new(peripherals.IO_MUX);

    let mut i2c = I2c::new(peripherals.I2C0, Config::default())
        .unwrap()
        .with_sda(peripherals.GPIO11)
        .with_scl(peripherals.GPIO10);
    //SPI

    // test input for the PCNT
    let mut test_gpio = Output::new(peripherals.GPIO9, Level::High);

    //ADC Configuration
    type AdcCal = esp_hal::analog::adc::AdcCalBasic<esp_hal::peripherals::ADC1>;
    let analog_pin = peripherals.GPIO3;
    let mut adc1_config = AdcConfig::new();
    let mut adc1_pin = adc1_config.enable_pin(analog_pin, Attenuation::_11dB);
    let mut adc1 = Adc::new(peripherals.ADC1, adc1_config);
    //CAN configuration
    let can_tx_pin = peripherals.GPIO2;
    let can_rx_pin = peripherals.GPIO20;

    //change to normal mode and construct to new
    let mut can_config = twai::TwaiConfiguration::new(
        peripherals.TWAI0,
        can_rx_pin,
        can_tx_pin,
        CAN_BAUDRATE,
        TwaiMode::SelfTest,
    );

    let can_filter = SingleStandardFilter::new(b"xxxxxxxxxxx", b"x", [b"xxxxxxxx", b"xxxxxxxx"]);

    can_config.set_filter(can_filter);

    let mut can = can_config.start();

    let device_id = StandardId::new(0x12).unwrap(); //make ID into env var

    //PCNT Configuration
    let pcnt = Pcnt::new(peripherals.PCNT);
    let u0 = pcnt.unit0;
    let ch0 = &u0.channel0;
    let wheel_speed_sensor = Input::new(peripherals.GPIO4, Pull::Up);

    u0.set_high_limit(Some(255)).unwrap();
    u0.set_filter(Some(min(10u16 * 80, 1023u16))).unwrap();
    u0.clear();
    ch0.set_edge_signal(wheel_speed_sensor.peripheral_input());
    ch0.set_input_mode(channel::EdgeMode::Increment, channel::EdgeMode::Hold);
    u0.listen();
    u0.resume();

    esp_println::logger::init_logger_from_env();

    //Timer Config
    let timg0 = TimerGroup::new(peripherals.TIMG0);
    let mut periodic = PeriodicTimer::new(timg0.timer0);
    let _ = periodic.start(1_u64.secs()); //wrong time setting

    //Variables for the hyperloop
    let mut can_data: [u8; 8] = [0; 8];
    let mut pin_value: u16;
    let mut start: esp_hal::time::Instant;
    let mut end: esp_hal::time::Instant;
    let mut frame: EspTwaiFrame;
    //let mut extern_adc_value: [u8; 2] = [2; 2];
    let mut dlhr_data: [u8; 8] = [0; 8];

    //SPI
    let sclk = peripherals.GPIO0;
    let miso = peripherals.GPIO6;
    let cs = peripherals.GPIO5;

    let mut spi = spi_bitbang::new(sclk, miso, cs);

    loop {
        //read single shot of data from the DLHR
        let _ = i2c.write_read(41, &[0xAC], &mut dlhr_data);
        println!("{:?}", &dlhr_data);
        let extern_adc = spi.get_adc();
        println!("{:?}", extern_adc);
        frame = EspTwaiFrame::new_self_reception(device_id, &dlhr_data).unwrap();
        nb::block!(can.transmit(&frame)).unwrap();

        pin_value = nb::block!(adc1.read_oneshot(&mut adc1_pin)).unwrap();

        //println!("{:?}", extern_adc_value);
        //
        for _ in 0..5 {
            test_gpio.toggle(); //testing PCNT
        }

        can_data[..2].copy_from_slice(&pin_value.to_be_bytes());
        can_data[2..4].copy_from_slice(&u0.counter.clone().get().to_be_bytes());
        //can_data[4..6].copy_from_slice(&extern_adc_value);

        frame = EspTwaiFrame::new_self_reception(device_id, &can_data).unwrap();
        nb::block!(can.transmit(&frame)).unwrap();

        u0.clear();

        start = time::now();
        let _ = nb::block!(periodic.wait());
        end = time::now();
        println!("{}", end - start);
    }
}
