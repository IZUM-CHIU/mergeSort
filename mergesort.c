#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    time_t c_start, t_start, c_end, t_end;
    //time_t c_start, c_end;
    //t_start = time(NULL);
    c_start = clock();
    unsigned seed;
    seed = (unsigned)time(NULL);

    int num = atoi(argv[1]);
    printf("num = %d", num);
    int *arr = (int *)malloc(num * sizeof(int));
    //int arr[10000000];
    int ran;
    seed = (unsigned)time(NULL);
    srand(seed);
    if (argc < 2)
    {
        printf("./a.out 10 2 -- 0 to 9 sort and 2 thread.\n");
        return 0;
    }
    if (num < 2)
    {
        printf("Number cannot be less than 2.");
        return 0;
    }
    for (int i = 0; i < num;)
    {
        ran = rand() % num;
        /*int j = 0;
        while (j < i)
        {
            if (arr[j] == ran)
                break;
            j++;
        }
        if (j == i)*/
            arr[i++] = ran;
    }
    /*for (int i = 0; i < num; i++)
    {
        printf("%d ", arr[i]);
    }*/
    printf("1");
    merge(arr, 0, num - 1);
    /*for (int i = 0; i < num; i++)
    {
        printf("%d ", arr[i]);
    }*/
    //printf("\n");
    c_end = clock();
    //t_end = time(NULL);
    printf("The pause used %f s by clock()\n", difftime(c_end, c_start)/ 1000000);
    //printf("The pause used %f s by time()\n", difftime(t_end, t_start));
    return 0;
}
