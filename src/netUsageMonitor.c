/*
 * @Description: a simple network usage speed monitor in real time.
 * @version: 0.1
 * @Author: Ricardo Lu
 * @Date: 2021-05-29 16:23:14
 * @Last Editor: Ricardo Lu
 * @LastEditTime: 2021-05-29 17:09:58
 */

#include "netUsageMonitor.h"
#include <string.h>

static time_t net_previous_timeStamp;
static double net_dif_time;
static int net_off;
static int read_line_num;
static FILE *net_info_stream;
static char net_buffer[1024];
static char *read_line_return;
static char net_dev_name[32];

struct netUsage m_netUsage;

void net_update(char *net_dev){
    long long net_itemReceive = 0;
    long long net_itemTransmit = 0;
    int net_info_drop = 0;
    int flag = 1;

    m_netUsage.net_previous_receive_total = m_netUsage.net_current_receive_total;//初始化上一次接收总数据
    m_netUsage.net_previous_transmit_total = m_netUsage.net_current_transmit_total;//初始化上一次发送总数据

    //获取网络的初始化数据
    /*step1: 获取并重置游标位置为文件开头处（每次更新计算机的网络的信息时，必须重新读入，或者重置游标的默认位置）*/
    net_off = fseek(net_info_stream, 0, SEEK_SET);//获取并重置游标位置为文件流开头处，SEEK_CUR当前读写位置后增加net_off个位移量,重置游标位置比重新打开并获取文件流的资源要低的道理，应该都懂吧 0.0
 
    // 前两行为无效信息
    read_line_num = 1;//重置行数
    read_line_return = fgets (net_buffer, sizeof(net_buffer), net_info_stream);//读取第一行
    read_line_num++;
    read_line_return = fgets (net_buffer, sizeof(net_buffer), net_info_stream);//读取第二行
     
    while(flag == 1){
        read_line_num++;
        read_line_return = fgets (net_buffer, sizeof(net_buffer), net_info_stream);
        net_itemReceive = 0;
        net_itemTransmit = 0;
        if(read_line_return != NULL){
            sscanf( net_buffer,
                "%s%lld%d%d%d%d%d%d%d%lld",
                net_dev_name,
                &net_itemReceive,
                &net_info_drop,
                &net_info_drop,
                &net_info_drop,
                &net_info_drop,
                &net_info_drop,
                &net_info_drop,
                &net_info_drop,
                &net_itemTransmit);
            if (!strncmp(net_dev_name, net_dev, strlen(net_dev))) {
                m_netUsage.net_current_receive_total = net_itemReceive;//初始化接收字节总数
                m_netUsage.net_current_transmit_total = net_itemTransmit;//初始化发送字节总数
            }
        } else {
            flag = -1;     
        }
    }
}

//初始化
void net_init(char *net_dev){
    //读取数据
    read_line_return = "INIT"; //设置初始值，只要不为空字符串即可
    /*step1: 打开文件 （每次更新计算机的网络的信息时，必须重新读入，或者重置游标的默认位置）*/
    net_info_stream = fopen (NET_INFO_PATH, "r"); //以R读的方式打开文件再赋给指针fd
    net_off = fseek(net_info_stream, 0, SEEK_SET);//获取并重置游标位置为文件流开头处，SEEK_CUR当前读写位置后增加net_off个位移量

    m_netUsage.net_current_receive_total = 0;  //对当前网络接收数据总数清零，重新计算
    m_netUsage.net_current_transmit_total = 0;//对当前网络发送数据总数清零，重新计算
    net_update(net_dev);//更新当前计算机的网络的信息

    m_netUsage.net_receive_speed = 0;//初始化接收网速为：0
    m_netUsage.net_transmit_speed = 0;//初始化发送网速为：0
    net_previous_timeStamp = time(NULL);//开始获取初始化的时间戳
    net_dif_time = 0;//初始化时间差

    printf("\n[INIT NET] net_previous_timeStamp: %f\n", (double)net_previous_timeStamp);
    printf("\n[INIT NET] Receive Total: %lld bytes.\n", m_netUsage.net_current_receive_total);
    printf("\n[INIT NET] Receive Speed: %f bytes/s.\n", m_netUsage.net_receive_speed);
    printf("\n[INIT NET] Transmit Total: %lld bytes.\n", m_netUsage.net_current_transmit_total);
    printf("\n[INIT NET] Transmit Speed: %f bytes/s.\n", m_netUsage.net_transmit_speed);
    printf("*********************************************************************************************************\n\n"); 
}

void net_update_usage(char *net_dev, time_t net_current_timeStamp, struct netUsage *netUsage_t){
    double net_dif_time = (double)(net_current_timeStamp - net_previous_timeStamp);//计算时间差（单位：秒）

    if( (net_dif_time) >= NET_DIFF_TIME ){//只有满足达到时间戳以后，才更新接收与发送的网络字节数据信息   
        net_update(net_dev);
        m_netUsage.net_transmit_speed = (m_netUsage.net_current_transmit_total - m_netUsage.net_previous_transmit_total) / net_dif_time;//更新发送网速(单位：字节/秒)
        m_netUsage.net_receive_speed = (m_netUsage.net_current_receive_total - m_netUsage.net_previous_receive_total) / net_dif_time;//更新接收网速(单位：字节/秒)
        //更新时间戳
        net_previous_timeStamp = net_current_timeStamp;
    }

    *netUsage_t = m_netUsage;
}

//关闭网络监控
void net_close(){
    fclose(net_info_stream);     //关闭文件net_stream
}