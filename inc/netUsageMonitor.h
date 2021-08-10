/*
 * @Description: 
 * @version: 
 * @Author: Ricardo Lu
 * @Date: 2021-05-29 16:23:14
 * @Last Editor: 
 * @LastEditTime: 2021-05-29 17:19:52
 */
#ifndef __NET_USAGE_MONITOR__
#define __NET_USAGE_MONITOR__

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NET_INFO_PATH   "/proc/net/dev"
#define NET_DIFF_TIME   1

struct netUsage {
    char net_dev[32];

    long long net_current_receive_total;
    long long net_previous_receive_total;
    double net_receive_speed;
    long long net_current_transmit_total;
    long long net_previous_transmit_total;
    double net_transmit_speed;
};

/**
 * @brief: init a netUsage struct
 * @param {char} *net_dev
 * @return {void}
 */
void net_init(char *net_dev);

/**
 * @brief: update info of specified network card 
 * @param {char} *net_dev
 * @return {*}
 */
void net_update(char *net_dev);

/**
 * @brief: update current speed of specified network and return it by netUsage_t
 * @param {char} *net_dev - network card name
 * @param {time_t} net_current_timeStamp - network update time stamp
 * @param {struct netUsage} *netUsage_t - return info struct
 * @return -void
 */
void net_update_usage(char *net_dev, time_t net_current_timeStamp, struct netUsage *netUsage_t);

/**
 * @brief: close the file read stream.
 * @param
 * @return - void
 */
void net_close();

#endif