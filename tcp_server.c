#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

int tnum = 0, s = 0;
int buf[2000000];

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

void *create_socket(void *arg)
{
    struct dt *t = (struct dt *)arg;
    int read_size;
    //Receive a message from client
    //send(socket, buffer, buffer_len, flag)
    /*for (int i = 0; i < t->n; i++)
    {
        printf("%d ", t->array[i]);
    }
    printf("\n");*/
    if (send(t->client_sock, (int *)&(t->n), sizeof(t->n), 0) < 0)
    {
        puts("Send num fail.");
        exit(EXIT_FAILURE);
    }
    if (send(t->client_sock, (int *)(t->array), t->n * sizeof(int), 0) < 0)
    {
        puts("Send array fail.");
        exit(EXIT_FAILURE);
    }
    int buffer[2000000];
    read_size = recv(t->client_sock, buffer, t->n * sizeof(int), 0);
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
    t->array = (int *)buffer;
    /*for (int i = 0; i < t->n; i++)
    {
        printf("%d ", t->array[i]);
    }
    printf("\n");*/
    for (int i = 0; i < t->n; i++, s++)
    {
        buf[s] = t->array[i];
    }
}

int main(int argc, char **argv)
{
    time_t t_start,t_end;
    int server_sock;
    struct sockaddr_in server, client;
    unsigned seed;
    seed = (unsigned)time(NULL);
    srand(seed);                
    if (argc < 2)
    { 
        printf("./a.out 10 2 -- 0 to 9 sort and 2 thread.\n");
        return 0;
    }
    if (atoi(argv[1]) < 2)
    { 
        printf("Number cannot be less than 2.");
        return 0;
    }
    int num = atoi(argv[1]);
    int *arr = (int *)malloc(num * sizeof(int));
    int ran;
    for (int i = 0; i < num;)
    {
        ran = rand() % num;
        //int j = 0;
       /* while (j < i)
        {
            if (arr[j] == ran)
                break;
            j++;
        }
        if (j == i) */
        arr[i++] = ran;
    }
    /*for (int i = 0;i < num; i++) { 
		ran = rand() % num;
		int j = 0;
		while(j < (i + 1)){  
			if(arr[j] == ran){ 
				ran = rand() % num;
				j = 0;
			}
			else
				j++;
		}
		arr[i] = ran;
	}*/
    
    if (atoi(argv[2]) < 2)
    {
        printf("Thread number cannot less than 2");
        return 0;
    }
    tnum = atoi(argv[2]); 

    //Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    //AF_INET -> IPV4 protocol, SOCK_STREAM -> Bitstream tran(TCP)
    if (server_sock == -1)
    {
        printf("Could't create socket!");
        exit(EXIT_FAILURE);
    }
    puts("Socket created!");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[3])); //htons -> host to net short int(16 bits)

    //Bind(sockfd, addr, addrlen)
    //sockfd -> socket_desc, addr -> server all info, addr_len -> server size
    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed. Error!");
        exit(EXIT_FAILURE);
    }
    puts("Bind done!");

    //Listen
    //listen(socket, backlog)
    //backlog ->How many people can connect to server
    if (listen(server_sock, tnum) < 0)
    {
        perror("error: listen");
        exit(1);
    }

    //Accpet and incoming connection
    puts("Waiting for incoming connections...");

    int ret = 0;
    int start = 0;
    pthread_t pthread[tnum];
    struct dt thread[tnum];
    t_start = clock();
    for (int i = 0; i < tnum; i++)
    {
        int c = sizeof(struct sockaddr_in);
        //accept connection from an incoming client
        thread[i].client_sock = accept(server_sock, (struct sockaddr *)&client, (socklen_t *)&c);

        if (thread[i].client_sock < 0)
        {
            perror("Accept failed!");
            exit(EXIT_FAILURE);
        }
        puts("Connection accpeted!");

        thread[i].tid = i;
        if (num % tnum == 0)          
            thread[i].n = num / tnum; 
        else
        {
            if ((num % tnum) > i)
                thread[i].n = (num / tnum) + 1;
            else
                thread[i].n = num / tnum;
        }
        thread[i].array = (int *)malloc(thread[i].n * sizeof(int));

        for (int j = 0; j < thread[i].n; j++, start++)
        { 
            thread[i].array[j] = arr[start];
        }
        ret = pthread_create(&pthread[i], NULL, create_socket, &thread[i]);
        if (ret != 0)
        {
            printf("Thread create fail.");
            return 0;
        }
    }
    for (int i = 0; i < tnum; i++)
        pthread_join(pthread[i], NULL);
    start = 0;
    merge(buf, 0, num - 1);
    /*for (int i = 0; i < tnum; i++)
    { 
        printf("tid:%d\n", thread[i].tid);
        printf("n:%d\n", thread[i].n);
        printf("array:");
        for (int j = 0; j < thread[i].n; j++)
        {
            printf("%d ", thread[i].array[j]);
        }
        printf("\n");
    }
    for (int i = 0; i < num; i++)
    { 
        printf("%d ", buf[i]);
    }
    printf("\n");*/
    t_end = clock();
    printf("time:%f s\n",difftime(t_end,t_start) / 1000000);
    return 0;
}
