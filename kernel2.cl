__kernel void addVectors(__global const int *arr,__global const int *sendCount,	__global const int *disp,
							__global int *sumArr,__global int *recvArr,const int id) {
		
		int gid = get_global_id(0);
		if(gid==id){
			int count=0; //variable for indexing
			for(int i=0;i<10;i++){
				for(int j=0;j<sendCount[i];j++){
					recvArr[count+j]=arr[j+disp[i]];
				}
				count+=sendCount[i]+disp[i];
			}
			// int sum=0;
			// for(int i=0;i<10;i++){
			// 	sum+=sendCount[i]+disp[i];
			// }
			// for(int i=0;i<sum;i++){
			// 	printf("arr:%d %d\n",i,recvArr[i]);
			// } 
		}
		int count2=0; //variable for indexing
		if(gid==0){
			count2=0;
		}
		else {
			for(int i=0;i<gid;i++){
				count2+=sendCount[i]+disp[i];
			}
		}
		sumArr[gid]=0;
		for(int i=0;i<sendCount[gid];i++){
			sumArr[gid]+=recvArr[count2+i];
			// printf("gid is %d %d\n",recvArr[count2+i]);
		}
		printf("The sum of gid: %d is %d \n",gid,sumArr[gid]);
	}
