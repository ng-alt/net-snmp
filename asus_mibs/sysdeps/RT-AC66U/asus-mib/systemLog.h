/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf 14476 2006-04-18 17:36:51Z hardaker $
 */
#ifndef SYSTEMLOG_H
#define SYSTEMLOG_H

/* function declarations */
void init_systemLog(void);
FindVarMethod var_systemLog;
FindVarMethod var_wl2gStationTable;
FindVarMethod var_wl5gStationTable;
FindVarMethod var_dhcpLeasesTable;
FindVarMethod var_ipv6LanDevicesTable;
FindVarMethod var_routingTable;
FindVarMethod var_ipv6RoutingTable;
FindVarMethod var_portForwardingLogTable;
FindVarMethod var_activeConnectionTable;
    WriteMethod write_sysLogTftpSrv;
    WriteMethod write_sysLogTftpFile;
    WriteMethod write_sysLogSaveAct;
    WriteMethod write_wl2gRefreshSta;
    WriteMethod write_wl5gRefreshSta;

#endif /* SYSTEMLOG_H */
