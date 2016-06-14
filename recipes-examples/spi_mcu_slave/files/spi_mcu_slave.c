#include <stdio.h>
#include <stdint.h>
#include <linux/spi/spidev.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

#define EXAMPLE_SPEED 100000
#define EXAMPLE_SPIDEV_PATH "/dev/spidev2.0"
#define BUFFER_SIZE 4 /* this value must be exactly the same as spi_slave on MCU */


int transfer_message(int fd, struct spi_ioc_transfer *message_p)
{
    int result;
    uint8_t *rx_sign_p;
    /* call io control - spidev kernel module will 
     * transfer then 'message' */
    if ((result = ioctl(fd, SPI_IOC_MESSAGE(1), message_p) < 0))
    {
        fprintf(stderr, "cannot call ioctl, status is: %d \n", result);
        return -1;
    }
    rx_sign_p = (uint8_t*)message_p->rx_buf;
    /* not a dummy char - put to stdin */
    if (*rx_sign_p != 0xFF)
    {
        write(STDIN_FILENO, rx_sign_p, sizeof(*rx_sign_p));
    }
}


int main_loop(const char *dev_path, uint32_t speed)
{
    uint8_t tx_sign, rx_sign;
    int fd, result;
    struct spi_ioc_transfer message;

    if ((fd = open(dev_path, O_RDWR)) < 0)
    {
        fprintf(stderr, "cannot open '%s'\n", dev_path);
        return -1;
    }

    /* LPC SSP peripheral requires assert/deassert
     * CS for each word so we have to transfer
     * each character separately */
    message.tx_buf = &tx_sign;
    message.rx_buf = &rx_sign;
    message.len = sizeof(tx_sign);
    message.speed_hz = speed;
    message.bits_per_word = 8;

    /* note: CSPI mode should be set for CPHA0 CPOL0 by default */
    while (read(STDIN_FILENO, (void*)&tx_sign, sizeof(tx_sign)) > 0)
    {
        transfer_message(fd, &message);
        /* if transfered char is newline, flush the queue on slave */
        if (tx_sign == '\n')
        {
            tx_sign = 0xFF;
            for (int i = 0; i < BUFFER_SIZE; i++)
                transfer_message(fd, &message);
        }
    }

    close(fd);
    return 0;
}


int main(int argc, char *argv[])
{
    int status;
    status = main_loop(EXAMPLE_SPIDEV_PATH, EXAMPLE_SPEED);
    return status;
}

