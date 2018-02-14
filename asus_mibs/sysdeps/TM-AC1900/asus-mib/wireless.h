/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf 17716 2009-08-04 21:35:34Z dts12 $
 */
#ifndef WIRELESS_H
#define WIRELESS_H

/* function declarations */
void init_wireless(void);
FindVarMethod var_wireless;
FindVarMethod var_wl2gWepKeyTable;
FindVarMethod var_wbWl2gBridgeAPScanTable;
FindVarMethod var_wbWl2gRemoteAPTable;
FindVarMethod var_wl2gMacFilterTable;
FindVarMethod var_wl5gWepKeyTable;
FindVarMethod var_wbWl5gBridgeAPScanTable;
FindVarMethod var_wbWl5gRemoteAPTable;
FindVarMethod var_wl5gMacFilterTable;
    WriteMethod write_ssidWl2g;
    WriteMethod write_hideSSIDWl2g;
    WriteMethod write_wirelessModeWl2g;
    WriteMethod write_optimizedforXboxWl2g;
    WriteMethod write_protectionWl2g;
    WriteMethod write_channelBandwidthWl2g;
    WriteMethod write_controlChannelWl2g;
    WriteMethod write_extensionChannelWl2g;
    WriteMethod write_authenticationWl2g;
    WriteMethod write_wepEncryptionWl2g;
    WriteMethod write_keyIndexWl2g;
    WriteMethod write_passphraseWl2g;
    WriteMethod write_wpaEncryptionWl2g;
    WriteMethod write_wpaPreSharedKeyWl2g;
    WriteMethod write_keyRotationIntervalWl2g;
    WriteMethod write_enableWPSWl2g;
    WriteMethod write_switchFrequencyTo5g;
    WriteMethod write_clientPinCodeWl2g;
    WriteMethod write_wpsMethodWl2g;
    WriteMethod write_wbApModeWl2g;
    WriteMethod write_wbConnectRemoteBridegeAPWl2g;
    WriteMethod write_wbBridgeAPScanActWl2g;
    WriteMethod write_macFilterModeWl2g;
    WriteMethod write_radiusServerIPAddrWl2g;
    WriteMethod write_radiusPortWl2g;
    WriteMethod write_radiusConnectionSecretWl2g;
    WriteMethod write_radioWl2g;
    WriteMethod write_enableSchedWl2g;
    WriteMethod write_weekDaysEnableRadioWl2g;
    WriteMethod write_weekDaysRadioStartTimeHWl2g;
    WriteMethod write_weekDaysRadioStartTimeMWl2g;
    WriteMethod write_weekDaysRadioEndTimeHWl2g;
    WriteMethod write_weekDaysRadioEndTimeMWl2g;
    WriteMethod write_weekendEnableRadioWl2g;
    WriteMethod write_weekendRadioStartTimeHWl2g;
    WriteMethod write_weekendRadioStartTimeMWl2g;
    WriteMethod write_weekendRadioEndTimeHWl2g;
    WriteMethod write_weekendRadioEndTimeMWl2g;
    WriteMethod write_apIsolatedWl2g;
    WriteMethod write_roamingAssistantWl2g;
    WriteMethod write_disconnectClientRSSIWl2g;
    WriteMethod write_igmpSnoopingWl2g;
    WriteMethod write_multicastRateWl2g;
    WriteMethod write_preambleTypeWl2g;
    WriteMethod write_aMPDURTSWl2g;
    WriteMethod write_rtsThresholdWl2g;
    WriteMethod write_dtimIntervalWl2g;
    WriteMethod write_beaconIntervalWl2g;
    WriteMethod write_txBurstingWl2g;
    WriteMethod write_wmmAPSDWl2g;
    WriteMethod write_optimizeAMPDUAggregWl2g;
    WriteMethod write_optimizeAckSuppressionWl2g;
    WriteMethod write_txPowerAdjustmentWl2g;
    WriteMethod write_ssidWl5g;
    WriteMethod write_hideSSIDWl5g;
    WriteMethod write_wirelessModeWl5g;
    WriteMethod write_optimizedforXboxWl5g;
    WriteMethod write_channelBandwidthWl5g;
    WriteMethod write_controlChannelWl5g;
    WriteMethod write_authenticationWl5g;
    WriteMethod write_wepEncryptionWl5g;
    WriteMethod write_keyIndexWl5g;
    WriteMethod write_passphraseWl5g;
    WriteMethod write_wpaEncryptionWl5g;
    WriteMethod write_wpaPreSharedKeyWl5g;
    WriteMethod write_keyRotationIntervalWl5g;
    WriteMethod write_enableWPSWl5g;
    WriteMethod write_switchFrequencyTo2g;
    WriteMethod write_clientPinCodeWl5g;
    WriteMethod write_wpsMethodWl5g;
    WriteMethod write_wbApModeWl5g;
    WriteMethod write_wbConnectRemoteBridegeAPWl5g;
    WriteMethod write_wbBridgeAPScanActWl5g;
    WriteMethod write_macFilterModeWl5g;
    WriteMethod write_radiusServerIPAddrWl5g;
    WriteMethod write_radiusPortWl5g;
    WriteMethod write_radiusConnectionSecretWl5g;
    WriteMethod write_radioWl5g;
    WriteMethod write_enableSchedWl5g;
    WriteMethod write_weekDaysEnableRadioWl5g;
    WriteMethod write_weekDaysRadioStartTimeHWl5g;
    WriteMethod write_weekDaysRadioStartTimeMWl5g;
    WriteMethod write_weekDaysRadioEndTimeHWl5g;
    WriteMethod write_weekDaysRadioEndTimeMWl5g;
    WriteMethod write_weekendEnableRadioWl5g;
    WriteMethod write_weekendRadioStartTimeHWl5g;
    WriteMethod write_weekendRadioStartTimeMWl5g;
    WriteMethod write_weekendRadioEndTimeHWl5g;
    WriteMethod write_weekendRadioEndTimeMWl5g;
    WriteMethod write_apIsolatedWl5g;
    WriteMethod write_roamingAssistantWl5g;
    WriteMethod write_disconnectClientRSSIWl5g;
    WriteMethod write_igmpSnoopingWl5g;
    WriteMethod write_multicastRateWl5g;
    WriteMethod write_preambleTypeWl5g;
    WriteMethod write_fragThresholdWl5g;
    WriteMethod write_rtsThresholdWl5g;
    WriteMethod write_dtimIntervalWl5g;
    WriteMethod write_beaconIntervalWl5g;
    WriteMethod write_txBurstingWl5g;
    WriteMethod write_wmmWl5g;
    WriteMethod write_wmmNoAcknowledgementWl5g;
    WriteMethod write_wmmAPSDWl5g;
    WriteMethod write_optimizeAMPDUAggregWl5g;
    WriteMethod write_optimizeAckSuppressionWl5g;
    WriteMethod write_explicitBeamformingWl5g;
    WriteMethod write_txPowerAdjustmentWl5g;
    WriteMethod write_wepKeyValueWl2g;
    WriteMethod write_wbRemoteAPMACAddrWl2g;
    WriteMethod write_wbRemoteAPDelEntryWl2g;
    WriteMethod write_macFilterMACAddrWl2g;
    WriteMethod write_macFilterDelEntryWl2g;
    WriteMethod write_wepKeyValueWl5g;
    WriteMethod write_wbRemoteAPMACAddrWl5g;
    WriteMethod write_wbRemoteAPDelEntryWl5g;
    WriteMethod write_macFilterMACAddrWl5g;
    WriteMethod write_macFilterDelEntryWl5g;

#endif /* WIRELESS_H */
