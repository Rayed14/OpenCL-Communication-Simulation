//Rayed Muhammad Saeed
//20i-1822
//CS-F
//Assignment 3
//Question Number 2
//Host program

/*
Sources: http://www.eriksmistad.no/getting-started-with-opencl-and-gpu-computing/
*/
// openCL headers 
#ifdef __APPLE__
#else
#include <CL/cl.h>
#endif
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include<math.h>
#define MAX_SOURCE_SIZE (0x100000)

int main(int argc, char** argv) {
	srand(time(0));
	int SIZE = 10; //number of processes
    int buffSize=atoi(argv[1]);
	while(buffSize<20 || buffSize>200){
		printf("Enter the size of array: ");
		scanf("%d",&buffSize);
	}
    printf("Size is: %d\n",buffSize);

    int* arr = (int*)malloc(sizeof(int) * buffSize); //main array
    int* sendCount = (int*)malloc(sizeof(int) * SIZE); //send count array
    int* disp = (int*)malloc(sizeof(int) * SIZE); //discp array

	for (int i=0;i<buffSize;i++){
		arr[i]=rand() % 100 +1; //main randomly initilized
	}

    for(int i=0;i<SIZE;i++){
		sendCount[i]=rand() % buffSize +1; //send count randomly initialized
        disp[i]=rand()%buffSize +1; 
		while (sendCount[i]+disp[i]>buffSize){
			sendCount[i]=rand() % buffSize +1;
			disp[i]=rand()%buffSize +1;
		}
    }

	int sum1=0;
	int sum2=0;
	for(int i=0;i<SIZE;i++){
		sum1+=sendCount[i];
		sum2+=disp[i];
	}
	int sum3=sum1+sum2;

	int* sumArr=(int*)malloc(sizeof(int) * SIZE);
	int *recvArr=(int*)malloc(sizeof(int) * sum3);
	for(int i=0;i<sum3;i++){
		recvArr[i]=0;
	}

	for(int i=0;i<SIZE;i++){
		printf("%d + %d = %d \n",sendCount[i],disp[i],sendCount[i]+disp[i]);
	}

    // int sum=0;
    // for(int i=0;i<SIZE;i++){
    //     sum+=sendCount[i];
    // }
    // printf("Sum is: %d \n",sum);

	// Load kernel from file kernel.cl

	FILE* kernelFile;
	char* kernelSource;
	size_t kernelSize;

	kernelFile = fopen("kernel2.cl", "r");
	if (!kernelFile) {
		fprintf(stderr, "No file named kernel2.cl was found\n");

		exit(-1);
	}
	kernelSource = (char*)malloc(MAX_SOURCE_SIZE);
	kernelSize = fread(kernelSource, 1, MAX_SOURCE_SIZE, kernelFile);
	fclose(kernelFile);

	// Getting platform and device information
	cl_platform_id platformId = NULL;
	cl_device_id deviceID = NULL;
	cl_uint retNumDevices;
	cl_uint retNumPlatforms;
	cl_int ret = clGetPlatformIDs(1, &platformId, &retNumPlatforms);
	char* value;
	size_t valueSize;

	ret = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_DEFAULT, 1, &deviceID, &retNumDevices);
	clGetDeviceInfo(deviceID, CL_DEVICE_NAME, 0, NULL, &valueSize);
	value = (char*)malloc(valueSize);
	clGetDeviceInfo(deviceID, CL_DEVICE_NAME, valueSize, value, NULL);
	// printf("Device: %s\n", value);
	free(value);

	// Creating context.
	cl_context context = clCreateContext(NULL, 1, &deviceID, NULL, NULL, &ret);

	// Creating command queue
	cl_command_queue commandQueue = clCreateCommandQueue(context, deviceID, 0, &ret);

	// Memory buffers for each array
	cl_mem arrMem = clCreateBuffer(context, CL_MEM_READ_ONLY, buffSize * sizeof(int), NULL, &ret);
	cl_mem sendMem = clCreateBuffer(context, CL_MEM_READ_ONLY, SIZE * sizeof(int), NULL, &ret);
	cl_mem dispMem = clCreateBuffer(context, CL_MEM_READ_ONLY, SIZE * sizeof(int), NULL, &ret);
	cl_mem sumMem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, SIZE * sizeof(int), NULL, &ret);
	cl_mem rcvMem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sum3 * sizeof(int), NULL, &ret);

	// Copy lists to memory buffers
    ret = clEnqueueWriteBuffer(commandQueue, arrMem, CL_TRUE, 0, buffSize * sizeof(int), arr, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(commandQueue, sendMem, CL_TRUE, 0, SIZE * sizeof(int), sendCount, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(commandQueue, dispMem, CL_TRUE, 0, SIZE * sizeof(int), disp, 0, NULL, NULL);
	// ret = clEnqueueWriteBuffer(commandQueue, sumMem, CL_TRUE, 0, SIZE * sizeof(int), sumArr, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(commandQueue, rcvMem, CL_TRUE, 0, sum3 * sizeof(int), recvArr, 0, NULL, NULL);

	// Create program from kernel source
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, (const size_t*)&kernelSize, &ret);

	// Build program
	ret = clBuildProgram(program, 1, &deviceID, NULL, NULL, NULL);

	// Create kernel
	cl_kernel kernel = clCreateKernel(program, "addVectors", &ret);

	int coordinatingID=rand()%9+0; //randomly assigning coordinating process
	printf("coordinating process is: %d\n",coordinatingID); //printing the coordinating process

	// Set arguments for kernel
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&arrMem);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&sendMem);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&dispMem);
	ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&sumMem);
	ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&rcvMem);
	ret = clSetKernelArg(kernel, 5, sizeof(int), (void*)&coordinatingID);

	// Execute the kernel
	size_t globalItemSize = SIZE;
	size_t localItemSize = SIZE; // globalItemSize has to be a multiple of localItemSize. 1024/64 = 16 
	ret = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &globalItemSize, &localItemSize, 0, NULL, NULL);

	// Read from device back to host.
	ret = clEnqueueReadBuffer(commandQueue, sumMem, CL_TRUE, 0, SIZE * sizeof(int), sumArr, 0, NULL, NULL);

	int result=0; 
	for(int i=0;i<10;i++){ //calculating the sum of the sums of array
		result+=sumArr[i];
	}
	printf("The final sum of array is: %d\n",result);	//printing the final answer

	// Clean up, release memory.
	ret = clFlush(commandQueue);
	ret = clFinish(commandQueue);
	ret = clReleaseCommandQueue(commandQueue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(arrMem);
	ret = clReleaseMemObject(sendMem);
	ret = clReleaseMemObject(dispMem);
	ret = clReleaseMemObject(sumMem);
	ret = clReleaseMemObject(rcvMem);
	ret = clReleaseContext(context);

	free(arr);
	free(sendCount);
	free(disp);
	free(sumArr);
	free(recvArr);

	return 0;
}
