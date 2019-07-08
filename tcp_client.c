#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

struct dt
{ 
    int tid;
    int client_sock;
    int n;
    int *array;
};

int *merge(int *arr, int l, int r)
{
    int mid = (r + l) / 2;
    if (l < r)
    { //divide
        merge(arr, l, mid);
        int left[mid - l + 1];
        for (int i = 0, j = l; i < mid - l + 1;)
            left[i++] = arr[j++];
        merge(arr, mid + 1, r);
        int right[r - mid];
        for (int i = 0, j = mid + 1; i < r - mid;)
            right[i++] = arr[j++];

        for (int i = 0, j = 0, k = l; k <= r;)
        { //conquer
            if ((i < (mid - l + 1)) && (j < (r - mid)))
            {
                if (left[i] < right[j])
                    arr[k++] = left[i++];
                else
                    arr[k++] = right[j++];
            }
            else
            {
                if (i < (mid - l + 1))
                    arr[k++] = left[i++];
                if (j < (r - mid))
                    arr[k++] = right[j++];
            }
        }
    }
}

int main(int argc, char **argv)
{
    int sock;
    struct sockaddr_in server;

    //Create Socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could't create socket!");
    }
    puts("Socket created!");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    //connect to which ip
    server.sin_port = htons(atoi(argv[2]));

    //Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connect failed. Error!");
        exit(EXIT_FAILURE);
    }
    puts("Connected\n");

    int read_size;
    int n = 0;
    //recv(socket, buffer, buffer_len, flag)
    read_size = recv(sock, (int *)&n, sizeof(int), 0);
    if (read_size == 0)
    {
        puts("Server disconnected!");
        fflush(stdout);
    }
    else if (read_size == -1)
    {
        perror("Num recv failed!");
        exit(EXIT_FAILURE);
    }
    //int *arr = (int *)malloc(n * sizeof(int));
    int buffer[2000000];
    read_size = recv(sock, buffer, n * sizeof(int), 0);
    printf("read_size = %d\n", read_size);
    if (read_size == 0)
    {
        puts("Server disconnected!");
        fflush(stdout);
    }
    else if (read_size == -1)
    {
        perror("Array recv failed!");
        exit(EXIT_FAILURE);
    }
    int *arr = (int *)buffer;
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
    merge(arr, 0, n - 1);
    if (send(sock, (int *)arr, n * sizeof(int), 0) < 0)
    {
        puts("Send array fail.");
        exit(EXIT_FAILURE);
    }
    close(sock);
    return 0;
}
