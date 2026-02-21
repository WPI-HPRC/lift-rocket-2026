#include <Arduino.h>
#include "LoRaE22/RadioConfigDatatypes.h"

using namespace RadioConfigTypes;

RadioConfig radioConfig = {
    .frequency = 223625.0, //KHz
    .address = 0xFFFF,
    .networkId = 0x01,
    .encryptionKey = 0x0000, // can't encrypt on a ham band (except we can)
    .parityConfig = ParityConfig::Parity_8N1,
    .serialSpeed = SerialSpeeds::BAUD_115200,
    .airDataRate = AirDataRate::RATE_15600,
    .packetSize = PacketSize::SIZE_240,
    .ambientRSSIEnabled = EnableRSSIReadings::Enabled,
    .rssiReadingsEnabled = EnableRSSIReadings::Disabled,
    .txPower = TransmitPower::dBm24,
    .destination = Destination::Broadcast,
    .relayMode = RelayMode::RelayDisabled,
    .listenBeforeTxEnable = EnableListenBeforeTX::LBTDisabled,
    .worMode = WakeOnReceiveMode::NormalWOR,
    .worPeriod = WakeOnReceiveListenPeriod::TIME_500
};

