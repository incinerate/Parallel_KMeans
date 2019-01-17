// Implementation of the KMeans Algorithm
// reference: http://mnemstudio.org/clustering-k-means-example-1.htm

#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <ctime>
#include <sys/time.h>
#include <mpi.h>

using namespace std;

float** init_float_matrix(int rows, int cols)
{
    float **mat;
    mat = new float*[rows];
    for(int i = 0; i<rows; i++)
    {
        mat[i] = new float[cols];
    }
    return mat;
}

void results_writer(float **dataframe, int total_points, int rank)
{
    ofstream result_file;
	stringstream file_name;
	file_name<<"MPI_results_"<<rank<<".txt";
    result_file.open(file_name.str());
    int i;
    result_file << "Point\n";
	for(i = 0; i < total_points; i++)
    {
       	if(dataframe[i][0] != 0)
      		result_file<<i+1<<"\n";
   	}
}

void points_writer(float **dataframe, int total_points, int total_values, int K, 
		int max_iterations)
{
    ofstream points_file;
    char file_name[] = "test_points.txt";
    points_file.open(file_name);
    int i,j;
    points_file << total_points << " "<< total_values<<" "<<K<<" "<<max_iterations<<" 0\n";
	for(i = 0; i < total_points; i++)
    {
       	
		   for(j = 0; j < total_values; j++)
		   {
			   points_file<<dataframe[i][j]<<" ";
		   }
		   points_file<<"\n";
   	}
}

float** file_reader(char file_name[], int total_points, int total_values)
{
    ifstream txt_file;
    txt_file.open(file_name, ios::in);
    char str[210];
    int start = 0, end, row_num = 0, col_num, first= 1;
    string row_values, substr, delim;
	string s;
    float** feature_matrix = init_float_matrix(total_points, total_values);
    while(getline(txt_file, s))
    {
		if(first) {
			first = 0;
			continue;
		}
		row_values = s;
        delim = " ";
        start = 0;
        end = row_values.find(delim);
        col_num = 0;
		int i;
        for(i=0; i<total_values; i++)
        {
            substr =  row_values.substr(start, end - start);
            feature_matrix[row_num][col_num++] = stof(substr);
            start = end + delim.length();
            end = row_values.find(delim, start);
		}
 		// feature_matrix[row_num++][col_num++] = stof(row_values.substr(start, end));
		row_num++;
    }
    return feature_matrix;
}

float euclidean_distance(float point[], float centre[], int d)
{
    int i;
    float distance = 0;
    for(i=1; i<d; i++)
    {
        distance = distance + ((point[i] - centre[i-1]) * (point[i] - centre[i-1]));
    }
    return distance/d;
}

float* assign_points_to_clusters(float **dataframe, float *min_distances, float *distances, int total_points, int total_values, int rank)
{
    int i, j, count = 0;
	float *mean_cluster = new float[total_values-1];
	for(i=0; i<total_values; i++)
		mean_cluster[i] = 0;
	for(i=0; i<total_points; i++)
	{
		if(min_distances[i] == distances[i])
		{
			dataframe[i][0] = rank+1;
			for(j=0; j<total_values-1; j++)
				mean_cluster[j] += dataframe[i][j+1];
			count++;
		}
		else
			dataframe[i][0] = 0;
	}
	for(j=0; j<total_values-1; j++)
		mean_cluster[j] /= count;
	return mean_cluster;
}

float* find_maxValue(float **feauture_matrix, int total_points, int total_values)
{
    //Returns a float[total_values-1] array containing maximum dimension value in all the samples.
    float *max_vals = new float[total_values-1];
    float max;
    for(int j = 1; j < total_values; j++)
    {
        max = 0.0;
        for(int i = 0; i < total_points; i++)
        {
            if(max < feauture_matrix[i][j])
            {
                max = feauture_matrix[i][j];
            }
        }
        max_vals[j-1] = max;
    }
    return max_vals;
}

float* init_clusters(float *rand_max, int total_values, int seed)
{
    float *clusters = new float[total_values-1];
    for(int j = 0; j < total_values-1; j++)
    {
        clusters[j] = rand_max[j] >= 1.0 ? (float) (rand() % (int) rand_max[j]) : 0.0;
    	clusters[j] = (clusters[j] * (seed^7)) / ((seed-1)^7);
    }
    return clusters;
}

float* calculate(float **dataframe, float *cluster, int total_points, int total_values)
{
	int i;
	float *distances = new float[total_points];
	for(i = 0; i < total_points; i++)
	{
		distances[i] = euclidean_distance(dataframe[i], cluster, total_values);
	}
	return distances;
}

int main(int argc, char *argv[])
{
	
	srand (time(NULL));


	int total_points = 15000, 
		total_values = 4, 
		K = 6, 
		max_iterations = 500, 
		has_name = 0;

	int rank, size;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	cout<<"rank:"<<rank<<endl;
	cout<<"size:"<<size<<endl;


	float *new_cluster = new float[total_values-1];
	float *cluster_centre;
	float **dataframe = init_float_matrix(total_points, total_values);
	float *distances;


	// dataframe = file_reader(file_name, total_points, total_values);

	for(int i = 0; i< total_points; i++){
		for(int j = 0; j< total_values; j++){
			dataframe[i][j] =(float) (rand() % 100)/10.0;
			// cout<<j<<"  "<<dataframe[i][j]<<" ";
		}
	}

	points_writer(dataframe, total_points, total_values, K, max_iterations);
		
    float *rand_max = find_maxValue(dataframe, total_points, total_values);
	MPI_Barrier(MPI_COMM_WORLD);
    cluster_centre = init_clusters(rand_max, total_values, rank+2);
	int changed = 1, ch = 0;
	float *min_distances = new float[total_points];
	
	struct timeval start;
	gettimeofday(&start, NULL);

	while(changed)
	{
		distances = calculate(dataframe, cluster_centre, total_points, total_values);
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Reduce(distances, min_distances, total_points, MPI_FLOAT, MPI_MIN, 0, MPI_COMM_WORLD);
		MPI_Bcast(min_distances, total_points, MPI_FLOAT, 0, MPI_COMM_WORLD);
		new_cluster = assign_points_to_clusters(dataframe, min_distances, distances, total_points, total_values, rank);
		MPI_Barrier(MPI_COMM_WORLD);
		ch = 0;
		for(int i = 0; i < total_values-1; i++)
		{
			if(new_cluster[i] != cluster_centre[i])
			{
				ch = 1;
			}
			cluster_centre[i] = new_cluster[i];
		}
		MPI_Reduce(&ch, &changed, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		fflush(stdout);
		MPI_Bcast(&changed, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
	}
	struct timeval end;
	gettimeofday(&end, NULL);
	int time = (end.tv_sec - start.tv_sec) * 1000000 + ((int)end.tv_usec - (int)start.tv_usec);
	// run_time = (clock() - run_time) / CLOCKS_PER_SEC;
	if(rank == 0)
		cout<<"run_time:"<<time<<endl;
	results_writer(dataframe, total_points, rank);
	MPI_Finalize();
	
	return 0;
}