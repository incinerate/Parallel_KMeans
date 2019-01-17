#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <algorithm>
#include <ctime>
#include <sys/time.h>
using namespace std;

float euclidean_distance(float point[], float centre[], int dim)
{
    int i;
    float distance = 0;
    for(i=1; i<dim; i++)
    {
        distance = distance + ((point[i] - centre[i-1]) * (point[i] - centre[i-1]));
    }
    return distance/dim;
}

int assign_points_to_clusters(float distances[], int k)
{
    float min;
    min = distances[0];
    int i, cluster = 0;
    for(i=1; i<k; i++)
    {
        if(min > distances[i])
        {
            cluster = i;
            min = distances[i];
        }
    }
    return cluster;
}

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

float** reader(char file_name[], int total_points, int total_values, int has_name)
{
    ifstream txt_file;
    txt_file.open(file_name, ios::in);
    char str[210];
    float** feature_matrix = init_float_matrix(total_points, total_values);
    for(int i = 0; i < total_points; i++)
	{
		for(int j = 0; j < total_values; j++)
		{
			cin >>feature_matrix[i][j];
		}

		if(has_name)
		{
			cin >> has_name;
		}
	}
    return feature_matrix;
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

float** init_clusters(float *rand_max, int K, int total_values)
{
    float **clusters = init_float_matrix(K, total_values-1);
    for(int i = 0; i < K; i++)
    {
        for(int j = 0; j < total_values-1; j++)
        {
            clusters[i][j] = rand_max[j] >= 1.0 ? (float) (rand() % (int) rand_max[j]) : 0.0;
        }
    }
    return clusters;
}

void clustering(float **dataframe, float **clusters, int total_points, int K, int total_values)
{
    int changed = 1, i, j, k, cluster, iter = 1;
    while(changed)
    {
        float distances[K], point_summation[K][total_values];
        for(i = 0; i < K; i++)
            for(j = 0; j < total_values; j++)
                point_summation[i][j] = 0;
                
        for(int i = 0; i < total_points; i++)
        {
            for(j = 0; j < K; j++)
            {
                distances[j] = euclidean_distance(dataframe[i], clusters[j], total_values);
            }
            cluster = assign_points_to_clusters(distances, K);
            dataframe[i][0] = cluster;
            point_summation[cluster][0]++;
            for(j = 1; j < total_values; j++)
                point_summation[cluster][j] += dataframe[i][j];
        }
        changed = 0;
        for(i = 0; i < K; i++)
        {
            for(j = 1; j < total_values; j++)
            {
                float new_value = point_summation[i][0] != 0?  point_summation[i][j] / point_summation[i][0]: 100;
                if(new_value != clusters[i][j-1])
                    changed = 1;
                clusters[i][j-1] = new_value;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    char file_name[] = "test_points.txt";
    int total_points, total_values, K, max_iterations, has_name;
    cin >> total_points >> total_values >> K >> max_iterations >> has_name;
    float **dataframe = reader(file_name, total_points, total_values, 0);

	struct timeval start;
	gettimeofday(&start, NULL);

    float **cluster_centres = init_clusters(find_maxValue(dataframe, total_points, total_values), K, total_values);
    clustering(dataframe, cluster_centres, total_points, K, total_values);

    struct timeval end;
	gettimeofday(&end, NULL);
	int time = (end.tv_sec - start.tv_sec) * 1000000 + ((int)end.tv_usec - (int)start.tv_usec);
    cout<<"run_time:"<<time<<endl;
    return 0;
}