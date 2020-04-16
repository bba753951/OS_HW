#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#define NETLINK_TEST    30
#define MSG_LEN            125
#define MAX_PLOAD        125

typedef struct _user_msg_info
{
    struct nlmsghdr hdr;
    char  msg[MSG_LEN];
} user_msg_info;

int main(int argc, char **argv)
{
    int skfd;
    int ret;
    user_msg_info u_info;
    socklen_t len;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl saddr, daddr;
    char umsg[1000] = {0};
    char sendmsg[1000]= {0};
    // char *fmsg = "Fail";
    int x=0;
    char *smsg = "Success";
    char *mailPID = argv[1];
    char *mailtype = argv[2];

    strcpy(umsg,mailPID);
    strcat(umsg,",");
    strcat(umsg,mailtype);


    /* 建立NETLINK socket */
    skfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if(skfd == -1)
    {
        perror("create socket error\n");
        return -1;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.nl_family = AF_NETLINK; //AF_NETLINK
    saddr.nl_pid = getpid();  //埠號(port ID)  100
    saddr.nl_groups = 0;
    if(bind(skfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
    {
        perror("bind() error\n");
        close(skfd);
        return -1;
    }

    memset(&daddr, 0, sizeof(daddr));
    daddr.nl_family = AF_NETLINK;
    daddr.nl_pid = 0; // to kernel
    daddr.nl_groups = 0;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = saddr.nl_pid; //self port

    memcpy(NLMSG_DATA(nlh), umsg, strlen(umsg));
    ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&daddr, sizeof(struct sockaddr_nl));
    if(!ret)
    {
        perror("sendto error\n");
        close(skfd);
        exit(-1);
    }
    // printf("send kernel:%s\n", umsg);

    memset(&u_info, 0, sizeof(u_info));
    len = sizeof(struct sockaddr_nl);
    ret = recvfrom(skfd, &u_info, sizeof(user_msg_info), 0, (struct sockaddr *)&daddr, &len);
    if(!ret)
    {
        perror("recv form kernel error\n");
        close(skfd);
        exit(-1);
    }

    // printf("from kernel:%s\n", u_info.msg);
    printf("%s\n", u_info.msg);
    if(strcmp(u_info.msg,smsg)==0)
    {
        // close(skfd);
        char cmd[10] = "";
        char mail_id[10] = "";
        char data[256] = "";

        while(1)
        {
            x++;
            x=scanf("%s",cmd);
            // printf("%s\n",cmd);
            memset(&sendmsg, 0, sizeof(sendmsg));
            if(!strcmp(cmd,"Send"))
            {

                x=scanf("%s %[^\n]",mail_id,data);
                // printf("ssend:%s,%s\n",mail_id,data);
                strcpy(sendmsg,mail_id);
                strcat(sendmsg,",");
                strcat(sendmsg,data);

                memset(NLMSG_DATA(nlh),0,NLMSG_SPACE(0));

                memcpy(NLMSG_DATA(nlh), sendmsg, strlen(sendmsg));
                ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&daddr, sizeof(struct sockaddr_nl));
                if(!ret)
                {
                    perror("sendto error\n");
                    close(skfd);
                    exit(-1);
                }
                // printf("send kernel:%s,%d\n", (char*)NLMSG_DATA(nlh),NLMSG_SPACE(0));

                memset(&u_info, 0, sizeof(u_info));
                len = sizeof(struct sockaddr_nl);
                ret = recvfrom(skfd, &u_info, sizeof(user_msg_info), 0, (struct sockaddr *)&daddr, &len);
                if(!ret)
                {
                    perror("recv form kernel error\n");
                    close(skfd);
                    exit(-1);
                }

                printf("%s\n", u_info.msg);
                // printf("from kernel:%s\n", u_info.msg);
            };
            if(!strcmp(cmd,"Recv"))
            {
                strcpy(sendmsg,mailPID);
                strcat(sendmsg,",");
                strcat(sendmsg,cmd);
                memset(NLMSG_DATA(nlh),0,NLMSG_SPACE(0));

                memcpy(NLMSG_DATA(nlh), sendmsg, strlen(sendmsg));
                ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&daddr, sizeof(struct sockaddr_nl));
                if(!ret)
                {
                    perror("sendto error\n");
                    close(skfd);
                    exit(-1);
                }
                // printf("send kernel:%s,%d\n", (char*)NLMSG_DATA(nlh),NLMSG_SPACE(0));

                memset(&u_info, 0, sizeof(u_info));
                len = sizeof(struct sockaddr_nl);
                ret = recvfrom(skfd, &u_info, sizeof(user_msg_info), 0, (struct sockaddr *)&daddr, &len);
                if(!ret)
                {
                    perror("recv form kernel error\n");
                    close(skfd);
                    exit(-1);
                }

                printf("%s\n", u_info.msg);
                // printf("from kernel:%s\n", u_info.msg);
            };
        }

        // memcpy(NLMSG_DATA(nlh), umsg, strlen(umsg));

        // printf("send kernel:%s\n", umsg);
    }

    close(skfd);

    free((void *)nlh);
    return 0;
}
