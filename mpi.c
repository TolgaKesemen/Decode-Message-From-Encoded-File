#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv) {
	
	int my_rank, comm_sz;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	int char_count = 0;
	char *lines = (char*) malloc(10000*sizeof(char));
	char *local_lines = (char*) malloc(1000*sizeof(char));
	strcpy(lines, "");

	if (my_rank == 0)
	{
		FILE *f = fopen(argv[1], "r");
		int c;
		int i = 0;
    		while ( ( c = fgetc(f) ) != EOF )
    		{
			*(lines+i) = (char) c;
			i++;
			char_count++;            		
    		}
		char_count /= comm_sz;	
    	}
	
	MPI_Bcast(&char_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(lines, char_count, MPI_CHAR, local_lines, char_count, MPI_CHAR, 0, MPI_COMM_WORLD);
	
	int local_key_count = 0;
	int key_count = 0;
	int j = 0;
	
	while( j < char_count )
	{
		if( *(local_lines+j) == *(argv[2]))
		{
			local_key_count++;
		}
		j++;
	}
		
	MPI_Allreduce(&local_key_count, &key_count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	j = 0;
	int k = 0;
	int n = 0;
	char *local_keys = (char*) malloc(char_count*sizeof(char));
	char *keys = (char*) malloc(char_count*sizeof(char));

	while( j < char_count)
	{
		if( *(local_lines+j) != '\n' )
		{
			if( ( ( (n+1) % key_count ) == 0 ) )
			{	
				*(local_keys+k) = *(local_lines+j);
				k++;
			}
			n++;
		}
		else {
			n = 0;
		}		
		j++;
	}
	
	MPI_Gather(local_keys,k,MPI_CHAR,keys,k,MPI_CHAR,0,MPI_COMM_WORLD);

	if(my_rank == 0)
	{
		printf("key: %s",keys);
	}
	MPI_Finalize();

    	return 0;
}