/*
 * This file was generated by mib2c and is intended for use as a mib module
 * for the ucd-snmp snmpd agent. 
 */


#ifndef _MIBGROUP_SNMPNOTIFYFILTERTABLE_H
#define _MIBGROUP_SNMPNOTIFYFILTERTABLE_H


/*
 * we may use header_complex from the header_complex module 
 */


config_require(header_complex)


    /*
     * our storage structure(s) 
     */
     struct snmpNotifyFilterTable_data {

         char           *snmpNotifyFilterProfileName;
         size_t          snmpNotifyFilterProfileNameLen;
         oid            *snmpNotifyFilterSubtree;
         size_t          snmpNotifyFilterSubtreeLen;
         char           *snmpNotifyFilterMask;
         size_t          snmpNotifyFilterMaskLen;
         long            snmpNotifyFilterType;
         long            snmpNotifyFilterStorageType;
         long            snmpNotifyFilterRowStatus;

     };




/*
 * enum definitions from the covered mib sections 
 */






#define SNMPNOTIFYFILTERTYPE_INCLUDED            1
#define SNMPNOTIFYFILTERTYPE_EXCLUDED            2




/*
 * function prototypes 
 */


     void            init_snmpNotifyFilterTable(void);
     FindVarMethod   var_snmpNotifyFilterTable;
     void            parse_snmpNotifyFilterTable(const char *, char *);
     SNMPCallback    store_snmpNotifyFilterTable;


     WriteMethod     write_snmpNotifyFilterMask;
     WriteMethod     write_snmpNotifyFilterType;
     WriteMethod     write_snmpNotifyFilterStorageType;
     WriteMethod     write_snmpNotifyFilterRowStatus;



#endif                          /* _MIBGROUP_SNMPNOTIFYFILTERTABLE_H */
