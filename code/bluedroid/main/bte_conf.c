/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  Filename:      bte_conf.c
 *
 *  Description:   Contains functions to conduct run-time module configuration
 *                 based on entries present in the .conf file
 *
 ******************************************************************************/

#define LOG_TAG "bte_conf"

#include <utils/Log.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "bt_target.h"
#include "bta_api.h"

#ifdef BLUETOOTH_RTK_COEX
#include "rtk_parse.h"
#endif

#ifdef BLUETOOTH_RTK
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include "bt_hci_bdroid.h"
#include "bdroid_buildcfg.h"
extern char bt_hci_device_node[BT_HCI_DEVICE_NODE_MAX_LEN];
#endif

/******************************************************************************
**  Externs
******************************************************************************/
extern BOOLEAN hci_logging_enabled;
extern char hci_logfile[256];
extern BOOLEAN trace_conf_enabled;
void bte_trace_conf(char *p_name, char *p_conf_value);
int device_name_cfg(char *p_conf_name, char *p_conf_value);
int device_class_cfg(char *p_conf_name, char *p_conf_value);
int logging_cfg_onoff(char *p_conf_name, char *p_conf_value);
int logging_set_filepath(char *p_conf_name, char *p_conf_value);
int trace_cfg_onoff(char *p_conf_name, char *p_conf_value);
#ifdef BLUETOOTH_RTK_COEX
int rtk_coex_log_onoff(char *p_conf_name, char *p_conf_value);
void bte_load_rtkbt_conf(const char *p_path);
int logging_set_rtkbt_filepath(char *p_conf_name, char *p_conf_value);

#endif

BD_NAME local_device_default_name = BTM_DEF_LOCAL_NAME;
DEV_CLASS local_device_default_class = {0x40, 0x02, 0x0C};

#ifdef BLUETOOTH_RTK
extern unsigned int  rtkbt_logfilter;
int set_rtkbt_logfilter(char *p_conf_name, char *p_conf_value);

#define USB_DEVICE_DIRECTORY  "/sys/bus/usb/devices"
#define DEBUG_SCAN_USB     FALSE
#endif

/******************************************************************************
**  Local type definitions
******************************************************************************/
#define CONF_DBG          0
#define info(format, ...) ALOGI (format, ## __VA_ARGS__)
#define debug(format, ...) if (CONF_DBG) ALOGD (format, ## __VA_ARGS__)
#define error(format, ...) ALOGE (format, ## __VA_ARGS__)

#define CONF_KEY_LEN   32
#define CONF_VALUE_LEN 96

#define CONF_COMMENT '#'
#define CONF_DELIMITERS " =\n\r\t"
#define CONF_VALUES_DELIMITERS "\"=\n\r\t"
#define CONF_COD_DELIMITERS " {,}\t"
#define CONF_MAX_LINE_LEN 255

typedef int (conf_action_t)(char *p_conf_name, char *p_conf_value);

typedef struct {
    const char *conf_entry;
    conf_action_t *p_action;
} conf_entry_t;

typedef struct {
    char key[CONF_KEY_LEN];
    char value[CONF_VALUE_LEN];
} tKEY_VALUE_PAIRS;

enum {
    CONF_DID,
    CONF_DID_RECORD_NUM,
    CONF_DID_PRIMARY_RECORD,
    CONF_DID_VENDOR_ID,
    CONF_DID_VENDOR_ID_SOURCE,
    CONF_DID_PRODUCT_ID,
    CONF_DID_VERSION,
    CONF_DID_CLIENT_EXECUTABLE_URL,
    CONF_DID_SERVICE_DESCRIPTION,
    CONF_DID_DOCUMENTATION_URL,
    CONF_DID_MAX
};
typedef UINT8 tCONF_DID;
/******************************************************************************
**  Static variables
******************************************************************************/

/*
 * Current supported entries and corresponding action functions
 */
/* TODO: Name and Class are duplicated with NVRAM adapter_info. Need to be sorted out */
static const conf_entry_t conf_table[] = {
    /*{"Name", device_name_cfg},
    {"Class", device_class_cfg},*/
    {"BtSnoopLogOutput", logging_cfg_onoff},
    {"BtSnoopFileName", logging_set_filepath},
    {"TraceConf", trace_cfg_onoff},
#ifdef BLUETOOTH_RTK_COEX
    {"BtCoexLogOutput", rtk_coex_log_onoff},
#endif
#ifdef BLUETOOTH_RTK
    {"RtkbtLogFilter", set_rtkbt_logfilter},
    {"BtDeviceNode", logging_set_rtkbt_filepath},
    {"DevClass", device_class_cfg},
#endif
    {(const char *) NULL, NULL}
};

static tKEY_VALUE_PAIRS did_conf_pairs[CONF_DID_MAX] = {
    { "[DID]",               "" },
    { "recordNumber",        "" },
    { "primaryRecord",       "" },
    { "vendorId",            "" },
    { "vendorIdSource",      "" },
    { "productId",           "" },
    { "version",             "" },
    { "clientExecutableURL", "" },
    { "serviceDescription",  "" },
    { "documentationURL",    "" },
};
/*****************************************************************************
**   FUNCTIONS
*****************************************************************************/

int device_name_cfg(char *p_conf_name, char *p_conf_value)
{
    strcpy((char *)local_device_default_name, p_conf_value);
    return 0;
}

int device_class_cfg(char *p_conf_name, char *p_conf_value)
{
    char *p_token;
    unsigned int x;

    p_token = strtok(p_conf_value, CONF_COD_DELIMITERS);
    sscanf(p_token, "%x", &x);
    local_device_default_class[0] = (UINT8) x;
    p_token = strtok(NULL, CONF_COD_DELIMITERS);
    sscanf(p_token, "%x", &x);
    local_device_default_class[1] = (UINT8) x;
    p_token = strtok(NULL, CONF_COD_DELIMITERS);
    sscanf(p_token, "%x", &x);
    local_device_default_class[2] = (UINT8) x;

    return 0;
}

int logging_cfg_onoff(char *p_conf_name, char *p_conf_value)
{
    if (strcmp(p_conf_value, "true") == 0)
        hci_logging_enabled = TRUE;
    else
        hci_logging_enabled = FALSE;
    return 0;
}

int logging_set_filepath(char *p_conf_name, char *p_conf_value)
{
    strcpy(hci_logfile, p_conf_value);
    return 0;
}

int trace_cfg_onoff(char *p_conf_name, char *p_conf_value)
{
    trace_conf_enabled = (strcmp(p_conf_value, "true") == 0) ? TRUE : FALSE;
    return 0;
}

/*BOARD_HAVE_BLUETOOTH_RTK_COEX begin*/
#ifdef BLUETOOTH_RTK_COEX
int rtk_coex_log_onoff(char *p_conf_name, char *p_conf_value)
{
    if (strcmp(p_conf_value, "true") == 0)
        coex_log_enable = 1;
    else
        coex_log_enable = 0;
    return 0;
}
#endif
/*BOARD_HAVE_BLUETOOTH_RTK_COEX end*/


/*****************************************************************************
**   CONF INTERFACE FUNCTIONS
*****************************************************************************/

/*******************************************************************************
**
** Function        bte_load_conf
**
** Description     Read conf entry from p_path file one by one and call
**                 the corresponding config function
**
** Returns         None
**
*******************************************************************************/
void bte_load_conf(const char *p_path)
{
    FILE    *p_file;
    char    *p_name;
    char    *p_value;
    conf_entry_t    *p_entry;
    char    line[CONF_MAX_LINE_LEN+1]; /* add 1 for \0 char */
    BOOLEAN name_matched;

    ALOGI("Attempt to load stack conf from %s", p_path);

    if ((p_file = fopen(p_path, "r")) != NULL)
    {
        /* read line by line */
        while (fgets(line, CONF_MAX_LINE_LEN+1, p_file) != NULL)
        {
            if (line[0] == CONF_COMMENT)
                continue;

            p_name = strtok(line, CONF_DELIMITERS);

            if (NULL == p_name)
            {
                continue;
            }

            p_value = strtok(NULL, CONF_VALUES_DELIMITERS);

            if (NULL == p_value)
            {
                ALOGW("bte_load_conf: missing value for name: %s", p_name);
                continue;
            }

            name_matched = FALSE;
            p_entry = (conf_entry_t *)conf_table;

            while (p_entry->conf_entry != NULL)
            {
                if (strcmp(p_entry->conf_entry, (const char *)p_name) == 0)
                {
                    name_matched = TRUE;
                    if (p_entry->p_action != NULL)
                        p_entry->p_action(p_name, p_value);
                    break;
                }

                p_entry++;
            }

            if ((name_matched == FALSE) && (trace_conf_enabled == TRUE))
            {
                /* Check if this is a TRC config item */
                bte_trace_conf(p_name, p_value);
            }
        }

        fclose(p_file);
    }
    else
    {
        ALOGI( "bte_load_conf file >%s< not found", p_path);
    }
}

#ifdef BLUETOOTH_RTK
int set_rtkbt_logfilter(char * p_conf_name,char * p_conf_value)
{
    rtkbt_logfilter = atoi(p_conf_value);
    return 0;
}

int Check_Key_Value(char* path,char* key,int value){
    FILE *fp;
    char newpath[100];
    char string_get[6];
    int value_int = 0;
    memset(newpath,0,100);
    sprintf(newpath,"%s/%s",path,key);
    if((fp=fopen(newpath,"r"))!=NULL){
        memset(string_get,0,6);
        if(fgets(string_get,5,fp)!=NULL)
            if(DEBUG_SCAN_USB)
                ALOGE("string_get %s =%s\n",key,string_get);
        fclose(fp);
        value_int = strtol(string_get,NULL,16);
        if(value_int == value)
            return 1;
    }
    return 0;
}

int Scan_Usb_Devices_For_RTK(char* path){
    char newpath[100];
    char subpath[100];
    DIR * pdir;
    DIR * newpdir;
    struct dirent * ptr;
    struct dirent * newptr;
    struct stat filestat;
    struct stat subfilestat;
    if(stat(path, &filestat) != 0){
        ALOGE("The file or path(%s) can not be get stat!\n", newpath);
        return -1;
    }
    if((filestat.st_mode & S_IFDIR) != S_IFDIR){
        ALOGE("(%s) is not be a path!\n", path);
        return -1;
    }
    pdir =opendir(path);
    /*enter sub direc*/
    while((ptr = readdir(pdir))!=NULL){
        if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        memset(newpath,0,100);
        sprintf(newpath,"%s/%s", path,ptr->d_name);
        if(DEBUG_SCAN_USB)
            ALOGE("The file or path(%s)\n", newpath);
        if(stat(newpath, &filestat) != 0){
            ALOGE("The file or path(%s) can not be get stat!\n", newpath);
            continue;
        }
        /* Check if it is path. */
        if((filestat.st_mode & S_IFDIR) == S_IFDIR){
            if(!Check_Key_Value(newpath,"idVendor",0x0bda))
                continue;
            newpdir =opendir(newpath);
            /*read sub directory*/
            while((newptr = readdir(newpdir))!=NULL){
                if(strcmp(newptr->d_name, ".") == 0 || strcmp(newptr->d_name, "..") == 0)
                    continue;
                memset(subpath,0,100);
                sprintf(subpath,"%s/%s", newpath,newptr->d_name);
                if(DEBUG_SCAN_USB)
                    ALOGE("The file or path(%s)\n", subpath);
                if(stat(subpath, &subfilestat) != 0){
                    ALOGE("The file or path(%s) can not be get stat!\n", newpath);
                    continue;
                }
                 /* Check if it is path. */
                if((subfilestat.st_mode & S_IFDIR) == S_IFDIR){
                    if(Check_Key_Value(subpath,"bInterfaceClass",0xe0) && \
                        Check_Key_Value(subpath,"bInterfaceSubClass",0x01) && \
                        Check_Key_Value(subpath,"bInterfaceProtocol",0x01)){
                        closedir(newpdir);
                        closedir(pdir);
                        return 1;
                    }
                }
            }
            closedir(newpdir);
        }
    }
    closedir(pdir);
    return 0;
}


int logging_set_rtkbt_filepath(char *p_conf_name, char *p_conf_value)
{
    strcpy(bt_hci_device_node, p_conf_value);
    if(bt_hci_device_node[0]=='?'){
        /*1.Scan_Usb_Device*/
        if(Scan_Usb_Devices_For_RTK(USB_DEVICE_DIRECTORY) == 0x01)
            strlcpy(bt_hci_device_node,"/dev/rtk_btusb", sizeof(bt_hci_device_node));
        else{
            int i = 0;
            while(bt_hci_device_node[i]!= '\0'){
                bt_hci_device_node[i] = bt_hci_device_node[i+1];
                i++;
            }
        }
    }
    ALOGI("bt_hci_device_node   = %s \n",bt_hci_device_node);

    return 0;
}
#endif
/*******************************************************************************
**
** Function        bte_parse_did_conf
**
** Description     Read conf entry from p_path file one by one and get
**                 the corresponding config value
**
** Returns         TRUE if success, else FALSE
**
*******************************************************************************/
static BOOLEAN bte_parse_did_conf (const char *p_path, UINT32 num,
    tKEY_VALUE_PAIRS *conf_pairs, UINT32 conf_pairs_num)
{
    UINT32 i, param_num=0, count=0, start_count=0, end_count=0, conf_num=0;
    BOOLEAN key=TRUE, conf_found=FALSE;

    FILE    *p_file;
    char    *p;
    char    line[CONF_MAX_LINE_LEN+1]; /* add 1 for \0 char */

    ALOGI("Attempt to load did conf from %s", p_path);

    if ((p_file = fopen(p_path, "r")) != NULL)
    {
        /* read line by line */
        while (fgets(line, CONF_MAX_LINE_LEN+1, p_file) != NULL)
        {
            count++;
            if (line[0] == CONF_COMMENT)
                continue;

            if (conf_found && (conf_num == num) && (*line == '[')) {
                conf_found = FALSE;
                end_count = count-1;
                break;
            }

            p = strtok(line, CONF_DELIMITERS);
            while (p != NULL) {
                if (conf_num <= num) {
                    if (key) {
                        if (!strcmp(p, conf_pairs[0].key)) {
                            if (++conf_num == num) {
                                conf_found = TRUE;
                                start_count = count;
                                strncpy(conf_pairs[0].value, "1", CONF_VALUE_LEN);
                            }
                        } else {
                            if (conf_num == num) {
                                for (i=1; i<conf_pairs_num; i++) {
                                    if (!strcmp(p, conf_pairs[i].key)) {
                                        param_num = i;
                                        break;
                                    }
                                }
                                if (i == conf_pairs_num) {
                                    error("Attribute %s does not belong to %s configuration",
                                        p, conf_pairs[0].key);
                                    fclose(p_file);
                                    return FALSE;
                                }
                            }
                            key = FALSE;
                        }
                    } else {
                        if ((conf_num == num) && param_num) {
                            strncpy(conf_pairs[param_num].value, p, CONF_VALUE_LEN-1);
                            param_num = 0;
                        }
                        key = TRUE;
                    }
                }
                p = strtok(NULL, CONF_DELIMITERS);
            }
        }

        fclose(p_file);
   }
   else
   {
        ALOGI( "bte_parse_did_conf file >%s< not found", p_path);
   }
   if (!end_count)
       end_count = count;

   if (start_count) {
        debug("Read %s configuration #%u from lines %u to %u in file %s",
            conf_pairs[0].key, (unsigned int)num, (unsigned int)start_count,
            (unsigned int)end_count, p_path);
        return TRUE;
   }

   error("%s configuration not found in file %s", conf_pairs[0].key, p_path);
        return FALSE;
}

/*******************************************************************************
**
** Function        bte_load_did_conf
**
** Description     Set local Device ID records, reading from configuration files
**
** Returns         None
**
*******************************************************************************/

void bte_load_did_conf (const char *p_path)
{
    tBTA_DI_RECORD rec;
    UINT32 rec_num, i, j;

    for (i=1; i<=BTA_DI_NUM_MAX; i++) {
        for (j=0; j<CONF_DID_MAX; j++) {
            *did_conf_pairs[j].value = 0;
        }

        if (bte_parse_did_conf(p_path, i, did_conf_pairs, CONF_DID_MAX)) {
            memset(&rec, 0, sizeof(rec));

            if (*did_conf_pairs[CONF_DID_RECORD_NUM].value) {
                rec_num = (UINT32)(strtoul(did_conf_pairs[CONF_DID_RECORD_NUM].value, NULL, 0)-1);
            } else {
                debug("[%d] Unknown %s", (unsigned int)i, did_conf_pairs[CONF_DID_RECORD_NUM].key);
                continue;
            }

            if (*did_conf_pairs[CONF_DID_VENDOR_ID].value) {
                rec.vendor = (UINT16)strtoul(did_conf_pairs[CONF_DID_VENDOR_ID].value, NULL, 0);
            } else {
                rec.vendor = LMP_COMPID_BROADCOM;
            }

            if (*did_conf_pairs[CONF_DID_VENDOR_ID_SOURCE].value) {
                rec.vendor_id_source = (UINT16)strtoul(did_conf_pairs[CONF_DID_VENDOR_ID_SOURCE].value, NULL, 0);
            } else {
                rec.vendor_id_source = DI_VENDOR_ID_SOURCE_BTSIG;
            }

            if ((*did_conf_pairs[CONF_DID].value == 0) ||
                (rec_num >= BTA_DI_NUM_MAX) ||
                (!((rec.vendor_id_source >= DI_VENDOR_ID_SOURCE_BTSIG) &&
                   (rec.vendor_id_source <= DI_VENDOR_ID_SOURCE_USBIF))) ||
                (rec.vendor == DI_VENDOR_ID_DEFAULT)) {

                error("DID record #%u not set", (unsigned int)i);
                for (j=0; j<CONF_DID_MAX; j++) {
                    error("%s:%s", did_conf_pairs[j].key, did_conf_pairs[j].value);
                }
                continue;
            }

            rec.product = (UINT16)strtoul(did_conf_pairs[CONF_DID_PRODUCT_ID].value, NULL, 0);
            rec.version = (UINT16)strtoul(did_conf_pairs[CONF_DID_VERSION].value, NULL, 0);

            strncpy(rec.client_executable_url,
                did_conf_pairs[CONF_DID_CLIENT_EXECUTABLE_URL].value,
                SDP_MAX_ATTR_LEN);
            strncpy(rec.service_description,
                did_conf_pairs[CONF_DID_SERVICE_DESCRIPTION].value,
                SDP_MAX_ATTR_LEN);
            strncpy(rec.documentation_url,
                did_conf_pairs[CONF_DID_DOCUMENTATION_URL].value,
                SDP_MAX_ATTR_LEN);

            for (j=0; j<strlen(did_conf_pairs[CONF_DID_PRIMARY_RECORD].value); j++) {
                did_conf_pairs[CONF_DID_PRIMARY_RECORD].value[j] =
                    tolower(did_conf_pairs[CONF_DID_PRIMARY_RECORD].value[j]);
            }
            if ((!strcmp(did_conf_pairs[CONF_DID_PRIMARY_RECORD].value, "true")) ||
                (!strcmp(did_conf_pairs[CONF_DID_PRIMARY_RECORD].value, "1"))) {
                rec.primary_record = TRUE;
            } else {
                rec.primary_record = FALSE;
            }

            info("[%u] primary_record=%d vendor_id=0x%04X vendor_id_source=0x%04X product_id=0x%04X version=0x%04X",
                (unsigned int)rec_num+1, rec.primary_record, rec.vendor,
                rec.vendor_id_source, rec.product, rec.version);
            if (*rec.client_executable_url) {
                info(" client_executable_url=%s", rec.client_executable_url);
            }
            if (*rec.service_description) {
                info(" service_description=%s", rec.service_description);
            }
            if (*rec.documentation_url) {
                info(" documentation_url=%s", rec.documentation_url);
            }

            if (BTA_DmSetLocalDiRecord(&rec, &rec_num) != BTA_SUCCESS) {
                error("SetLocalDiInfo failed for #%u!", (unsigned int)i);
            }
        }
    }
}

#ifdef BLUETOOTH_RTK
void bte_load_rtkbt_conf(const char *p_path)
{

    FILE    *p_file;
    char    *p_name;
    char    *p_value;
    conf_entry_t    *p_entry;
    char    line[CONF_MAX_LINE_LEN+1]; /* add 1 for \0 char */
    BOOLEAN name_matched;

    ALOGD("Attempt to load rtkbt conf from %s", p_path);

    if ((p_file = fopen(p_path, "r")) != NULL)
    {
        /* read line by line */
        while (fgets(line, CONF_MAX_LINE_LEN+1, p_file) != NULL)
        {
            if (line[0] == CONF_COMMENT)
                continue;

            p_name = strtok(line, CONF_DELIMITERS);

            if (NULL == p_name)
            {
                continue;
            }

            p_value = strtok(NULL, CONF_VALUES_DELIMITERS);

            if (NULL == p_value)
            {
                ALOGW("bte_load_rtkbt_conf: missing value for name: %s", p_name);
                continue;
            }

            name_matched = FALSE;
            p_entry = (conf_entry_t *)conf_table;

            while (p_entry->conf_entry != NULL)
            {
                if (strcmp(p_entry->conf_entry, (const char *)p_name) == 0)
                {
                    name_matched = TRUE;
                    if (p_entry->p_action != NULL)
                        p_entry->p_action(p_name, p_value);
                    break;
                }

                p_entry++;
            }
        }
        fclose(p_file);
    }
    else
    {
        ALOGI( "bte_load_rktb_conf file >%s< not found", p_path);
    }
}
#endif


