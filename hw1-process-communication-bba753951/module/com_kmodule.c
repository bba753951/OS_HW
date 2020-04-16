#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/string.h>
#include "com_kmodule.h"

#define NETLINK_TEST     30
#define MSG_LEN            125
#define USER_PORT        100
#define ID_COUNT        1000

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhangwj");
MODULE_DESCRIPTION("netlink example");

struct sock *nlsk = NULL;
extern struct net init_net;
int mail_state[ID_COUNT]= {0};
struct mailbox allbox[ID_COUNT];



int send_usrmsg(char *pbuf, uint16_t len ,__u32 po)
{
    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;

    int ret;

    /* 建立sk_buff 空間 */
    nl_skb = nlmsg_new(len, GFP_ATOMIC);
    if(!nl_skb)
    {
        printk("netlink alloc failure\n");
        return -1;
    }

    /* 設定netlink訊息頭部 */
    nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_TEST, len, 0);
    if(nlh == NULL)
    {
        printk("nlmsg_put failaure \n");
        nlmsg_free(nl_skb);
        return -1;
    }

    /* 拷貝資料傳送 */
    memcpy(nlmsg_data(nlh), pbuf, len);
    // ret = netlink_unicast(nlsk, nl_skb, USER_PORT, MSG_DONTWAIT);
    ret = netlink_unicast(nlsk, nl_skb, po, MSG_DONTWAIT);

    return ret;
}

static void netlink_rcv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = NULL;
    char *umsg = NULL;
    // char *kmsg = "hello users!!!";
    char *fmsg = "Fail";
    char *smsg = "Success";
    int data_state;    // 0 for queued ,1 for unqueued ,2 for data
    char* umsg1;
    int mail_id;
    char* const delim = ",";
    struct msg_data *nodedata=NULL;
    struct msg_data *deletenode=NULL;
    // struct msg_data *current=NULL;


    nodedata=(struct msg_data*)kmalloc(sizeof(struct msg_data),GFP_KERNEL);


    if(skb->len >= nlmsg_total_size(0))
    {
        nlh = nlmsg_hdr(skb);
        umsg = NLMSG_DATA(nlh);
        if(umsg)
        {
            printk("from user: %s\n", umsg);


            umsg1=strsep(&umsg,delim);
            sscanf(umsg1,"%d",&mail_id);
            mail_id--;
            if(!strcmp(umsg,"queued"))
            {
                data_state=0;
                allbox[mail_id].type='0';
            }
            else if(!strcmp(umsg,"unqueued"))
            {
                data_state=1;
                allbox[mail_id].type='1';
            }
            else if(!strcmp(umsg,"Recv"))
            {
                data_state=3;
            }
            else
                data_state=2;

            if(data_state == 2)
            {


                printk("send\n");
                strcpy(nodedata->buf,umsg);
                nodedata->next=NULL;
                printk("msg_data_count:%c\n",allbox[mail_id].msg_data_count);
                printk("type:%c\n",allbox[mail_id].type);
                if(allbox[mail_id].msg_data_count == '3')
                {
                    send_usrmsg(fmsg, strlen(fmsg),nlh->nlmsg_pid);
                    return;
                }
                if(allbox[mail_id].msg_data_count == '0')
                {


                    allbox[mail_id].msg_data_head=nodedata;
                    allbox[mail_id].msg_data_tail=nodedata;
                    allbox[mail_id].msg_data_count++;
                    send_usrmsg(smsg, strlen(smsg),nlh->nlmsg_pid);
                    return;
                }
                if(allbox[mail_id].type=='1')
                {
                    allbox[mail_id].msg_data_head=nodedata;
                    allbox[mail_id].msg_data_count='1';
                }
                else
                {
                    deletenode=allbox[mail_id].msg_data_head;
                    while (deletenode->next)             // Traversal
                    {
                        deletenode = deletenode->next;
                    }
                    deletenode->next=nodedata;
                    allbox[mail_id].msg_data_tail=nodedata;
                    allbox[mail_id].msg_data_count++;
                }


                send_usrmsg(smsg, strlen(smsg),nlh->nlmsg_pid);
                return;
            }
            if(data_state == 3)
            {
                // 收信
                printk("recv\n");
                printk("msg_data_count:%c\n",allbox[mail_id].msg_data_count);
                printk("type:%c\n",allbox[mail_id].type);

                if(allbox[mail_id].msg_data_count == '0')
                    send_usrmsg(fmsg, strlen(fmsg),nlh->nlmsg_pid);
                else
                {
                    printk("buf:%s",allbox[mail_id].msg_data_head->buf);
                    send_usrmsg(allbox[mail_id].msg_data_head->buf,strlen(allbox[mail_id].msg_data_head->buf),nlh->nlmsg_pid);
                    if(allbox[mail_id].type=='0')
                    {
                        allbox[mail_id].msg_data_count--;
                        deletenode=allbox[mail_id].msg_data_head;
                        allbox[mail_id].msg_data_head = allbox[mail_id].msg_data_head->next;
                        kfree(deletenode);
                    };
                }
                return;

            }


            if(mail_state[mail_id] == 1)
            {
                send_usrmsg(fmsg, strlen(fmsg),nlh->nlmsg_pid);
                return;
            }



            // 記得要改回1
            mail_state[mail_id]=1;

            printk("kernel recv from user: %s,%s,%d\n", umsg,umsg1,mail_id);
            send_usrmsg(smsg, strlen(smsg),nlh->nlmsg_pid);
        }
    }
}

struct netlink_kernel_cfg cfg =
{
    .input  = netlink_rcv_msg, /* set recv callback */
};

int test_netlink_init(void)
{

    int i;
    for(i=0; i<ID_COUNT; i++)
    {
        allbox[i].type='0';
        allbox[i].msg_data_count='0';
    }


    /* create netlink socket */
    nlsk = (struct sock *)netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
    if(nlsk == NULL)
    {
        printk("netlink_kernel_create error !\n");
        return -1;
    }
    printk("test_netlink_init\n");

    return 0;
}

void test_netlink_exit(void)
{
    if (nlsk)
    {
        netlink_kernel_release(nlsk); /* release ..*/
        nlsk = NULL;
    }
    printk("test_netlink_exit!\n");
}

module_init(test_netlink_init);
module_exit(test_netlink_exit);
