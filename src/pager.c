#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stumpless.h>
#include "safehouse/pager.h"

Pager *pager_open(const char *filename) {
    int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);

    if (fd == -1) {
        stump_c("Unable to open file");
        exit(EXIT_FAILURE);
    }

    off_t file_length = lseek(fd, 0, SEEK_END);
    Pager *pager = malloc(sizeof(Pager));
    pager->fd = fd;
    pager->file_length = file_length;

    for (size_t i = 0; i < TABLE_MAX_PAGES; i++)
        pager->pages[i] = NULL;
    return pager;
}

void *pager_get(Pager *pager, size_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        stump_c("Tried to fetch page number out of bounds. %ld > %ld", page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[page_num] == NULL) {
        void *page = malloc(PAGE_SIZE);
        size_t num_pages = pager->file_length / PAGE_SIZE;
        if (pager->file_length % PAGE_SIZE)
            num_pages++;
        if (page_num <= num_pages) {
            lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->fd, page, PAGE_SIZE);
            if (bytes_read == -1) {
                stump_c("Error reading file: %d", errno);
                exit(EXIT_FAILURE);
            }
            pager->pages[page_num] = page;
        }
    }
    return pager->pages[page_num];
}

void pager_flush(Pager *pager, size_t page_num, size_t size) {
    if (pager->pages[page_num] == NULL) {
        stump_c("Tried to flush null page");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        stump_c("Error seeking: %d", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(pager->fd, pager->pages[page_num], size);
    if (bytes_written == -1) {
        stump_c("Error writing: %d", errno);
        exit(EXIT_FAILURE);
    }
}