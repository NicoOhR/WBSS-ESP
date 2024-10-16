use embedded_can::StandardId;
use esp_idf_hal::can;
use esp_idf_svc::hal::adc::{AdcContConfig, AdcContDriver, AdcMeasurement, Attenuated};
use esp_idf_svc::hal::peripherals::Peripherals;
use log::debug;

fn main() -> anyhow::Result<()> {
    // It is necessary to call this function once. Otherwise some patches to the runtime
    // implemented by esp-idf-sys might not link properly. See https://github.com/esp-rs/esp-idf-template/issues/71
    esp_idf_svc::sys::link_patches();

    // Bind the log crate to the ESP Logging facilities
    esp_idf_svc::log::EspLogger::initialize_default();

    let peripherals = Peripherals::take()?;

    let adc_config = AdcContConfig::default();

    let adc_1_channel_0 = Attenuated::db11(peripherals.pins.gpio1);

    let mut adc_driver = AdcContDriver::new(peripherals.adc1, &adc_config, adc_1_channel_0)?;

    adc_driver.start()?;

    let timing = can::config::Timing::B250K; //250 baud rate
    let can_config = can::config::Config::new()
        .timing(timing)
        .mode(can::config::Mode::NoAck); //No ack for standalong testing
    let mut can_driver = can::CanDriver::new(
        peripherals.can,
        peripherals.pins.gpio5,
        peripherals.pins.gpio4,
        &can_config,
    )?;

    can_driver.start()?;
    let can_id = StandardId::new(0x041).unwrap();

    let mut samples: [AdcMeasurement; 100] = [Default::default(); 100];
    let tx_frame = embedded_can::Frame::new(can_id, &[0, 1, 2, 3, 4, 5, 6, 7, 8]).unwrap();

    loop {
        if let Ok(num_read) = adc_driver.read(&mut samples, 10) {
            debug!("Read {} measurement.", num_read);
            for index in 0..num_read {
                debug!("{}", samples[index].data());
            }
        }
        can_driver.transmit(&tx_frame, 500)?;
    }
}
