#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define I2C_ADDR 0x50
#define PAGE_SIZE 64
unsigned char rdbuf[256] = {0};
unsigned char wrbuf[256] = {0};
int IIC_Write(int fd, int16_t addr, unsigned char* buff, int len) {
    unsigned char* send_buf = (unsigned char*)malloc(PAGE_SIZE + sizeof(addr));
    int num = 0;
    if (len <= PAGE_SIZE) {
        memcpy(send_buf, &addr, sizeof(addr));
        memcpy(send_buf + sizeof(addr), buff, len);
        for (int i = 0; i < len + sizeof(addr); i++) {
            printf("send_buf[%d]=%d\n", i, send_buf[i]);
        }
        num = write(fd, send_buf, len + sizeof(addr));
        usleep(250000);
    } else {
        int i = 0;
        int total = len / PAGE_SIZE;
        printf("total=%d\n", total);
        for (i = 0; i < total; i++) {
            int16_t snd_addr = addr + i * PAGE_SIZE;
            memcpy(send_buf, &snd_addr, sizeof(addr));

            memcpy(send_buf + sizeof(addr), buff + i * PAGE_SIZE, PAGE_SIZE);
            num += write(fd, send_buf, PAGE_SIZE + sizeof(addr));
            usleep(250000);
        }
        int remain = len % PAGE_SIZE;
        printf("remain=%d\n", remain);
        int16_t snd_addr = addr + total * PAGE_SIZE;
        memcpy(send_buf, &snd_addr, sizeof(addr));
        memcpy(send_buf + sizeof(addr), buff + total * PAGE_SIZE, remain);
        num += write(fd, send_buf, remain + sizeof(addr));
        usleep(250000);
    }
    free(send_buf);
    printf("send num=%d\n", num);
    return num;
}

int IIC_PageRead(int fd, int16_t rdaddr, unsigned char* buff, int len) {
    write(fd, &rdaddr, sizeof(rdaddr));
    return read(fd, buff, len);
}

int main() {
    int fd = open("/dev/i2c-0", O_RDWR);
    if (fd < 0) {
        printf("err open file:%s\n", strerror(errno));
        return -1;
    }
    if (ioctl(fd, I2C_SLAVE_FORCE, I2C_ADDR) < 0) {
        printf("ioctl error : %s\n", strerror(errno));
        return -1;
    }
    int i = 0;
    int len = 16;
    sprintf(wrbuf, "%s", "{\"name\":\"lm\",\"type\":\"ad\"}");

    len = strlen(wrbuf);

    printf("wrbuf len=%d\n", len);
    int16_t addr = 0;
    if (IIC_Write(fd, addr, wrbuf, len) <= 0) {
        printf("Wrtie1 Error! %s\n", strerror(errno));
        return -1;
    }

    if (IIC_PageRead(fd, addr, rdbuf, len) > 0) {
        for (i = 0; i < len; i++) {
            printf("val[%d]=%x\n", i, rdbuf[i]);
        }
        printf("rdbuf=%s\n", rdbuf);
    } else {
        printf("Read Error! %s\n", strerror(errno));
        return -1;
    }
    return 0;
}