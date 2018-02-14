/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf 14476 2006-04-18 17:36:51Z hardaker $
 */
#ifndef IPV6_H
#define IPV6_H

/* function declarations */
void init_ipv6(void);
FindVarMethod var_ipv6;
    WriteMethod write_connectionType;
    WriteMethod write_nativeDhcpPd;
    WriteMethod write_serverIPv4Address;
    WriteMethod write_ipv4AnycastRelay;
    WriteMethod write_dhcpOption;
    WriteMethod write_ipv6Prefix;
    WriteMethod write_ipv6PrefixLength6rd;
    WriteMethod write_ipv4BorderRouter;
    WriteMethod write_ipv4RouterMaskLength;
    WriteMethod write_clientIPv6Address;
    WriteMethod write_ipv6PrefixLength6in4;
    WriteMethod write_tunnelMTU;
    WriteMethod write_tunnelTTL;
    WriteMethod write_wanIPv6Address;
    WriteMethod write_wanPrefixLength;
    WriteMethod write_wanIPv6Gateway;
    WriteMethod write_lanIpv6Address;
    WriteMethod write_lanIpv6PrefixLength;
    WriteMethod write_lanIpv6Prefix;
    WriteMethod write_lanIpv6AutoConfiguration;
    WriteMethod write_lanIpv6LifeTime;
    WriteMethod write_lanIpv6DhcpPoolStart;
    WriteMethod write_lanIpv6DhcpPoolEnd;
    WriteMethod write_ipv6ConnectToDNSServerAuto;
    WriteMethod write_ipv6DNSServer1;
    WriteMethod write_ipv6DNSServer2;
    WriteMethod write_ipv6DNSServer3;
    WriteMethod write_enableRouterAdvertisement;

#endif /* IPV6_H */
