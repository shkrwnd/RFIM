//	RFIM for 2D without OPENCV
//	making use of stl vectors instead of new 
//	with clustering implementation
//	for raw visited node output
//
//	author: Teknas    
// 	Not enough 
//  overtaken by Shikhar

//#define _HAS_ITERATOR_DEBUGGING 0
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include "Headers.h"
#include "Parameters.h"
#include "Clustering.h"
#include "PushRelabel.h"
#include "LatticeStuff.h"
#ifdef  unix
	#include <pthread.h>
#endif
#define NUM_THREADS  1

using namespace std;


ofstream f;

int randomizer(void){
    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_int_distribution<int> distr(0,5000);
    return distr(generator);
}
void *blowjob(void* thread_id) {
	pthread_mutex_t mutex;

	long id = (long)thread_id;
	string file_name = "log/log";
	file_name += to_string(id);
	file_name = file_name + "_" + to_string(randomizer()) +  ".dat";
	f.open(file_name,ios::app);

	cout << file_name << endl;

	time_t time_begin, time_end, time_1, time_2;

	long t1, t2, tdiff[iter] = { 0 }, l = 0, sum = 0, i, j, cap = 0, clusters = 0;
	
	for (l = 0; l < iter; l++)
	{
		f << "iter: " << l <<endl;
		cout << "\n\n*****ITER:  " << l << "****\n\n";
			
		
	/* ==================== INITIALISING 2D VECTORS ================*/
		vector < vector <int> >   flow(V, vector<int>(V, 0))
								, CapacityMat(V, vector<int>(V, 0))
								, Exmat(N, vector<int>(N, 0))
								, sqlat0(VER + 1, vector<int>(VER + 1, 0))
								, sqlat1(VER + 1, vector<int>(VER + 1, 0));

	/* ==================== INITIALISING 1D VECTORS ================*/
		vector <int>  visited(V, 0), Wmat(N, 0), latt(N, 1), Bmat(N, 0), clusstats0(V / 2, 0), clusstats1(V / 2, 0);

	/* =========================== BETA MATRIX (Bmat)======================= */
	
		//create_Bmat_bimodal(Bmat);  // UNCOMMENT FOR BIMODAL DISTRIBUTION
		create_Bmat_gaussian(Bmat);
		cout << "Bmat created" << endl;
		
	/* ====================== CREATING EXISTANCE MATRIX (Exmat)============= */

		create_Exmat(Exmat, latt);
		cout << "Exmat created" << endl;
		
	/* ================= CREATING CAPACITY MATRIX (CapacityMat)============= */

		create_CapacityMat(CapacityMat, Exmat);
		cout << "created CapacityMatrix" << endl;

	/* =================== LOOP TO ITERATE OVER RANGE OF DELTA ============= */

		for (int del = del_beg; del <= del_end; del += del_inc)
		{
			f << "del: " << del << endl;
			//open 
			cout << "\ndEL: " << float(del) / 10 << tab;

			create_Wmat(Wmat, CapacityMat, Bmat, del);

			/* =========== AGUMENTING CAPACITY MATRIX WITH WMAT ============= */

			create_Augumented_CapacityMat(Wmat, CapacityMat);

			t1 = time(&time_1);  /* get current time;*/

			/*================================================================================================*/
			/*  ladies and gentlemen its honour to present you the most important stuff in this awesome code  */
			/* ===================== CALLING PUSH RELABEL(CapacityMat)======================================= */

			pushRelabel(CapacityMat, flow, 0, V - 1);

			/*====================================================================================*/
			t2 = time(&time_2);  /* get current time;*/

			/* ====================== CREATE RESIDUAL GRAPH ================== */
			create_Residual_graph(CapacityMat, flow);

			/*======================================================*/
			/*============	CLUSTERIZATION 	======================= */
			/*======================================================*/

			/* ================= DEPTH FIRST SEARCH ON FLOW ================== */
			dfs(flow, 0, visited);

			/* ============ CREATE AGUMENTED MATRIX AROUND LATTICE =========== */
			createAgumentedMatrix(sqlat0, sqlat1, visited);

			/* ====================== HK CLUSTER ALGO ================== */
			cluster(sqlat1, 1, clusstats1);
			cluster(sqlat0, 0, clusstats0);

			/* ====================== PRINT CLUSTERS ================== */
				//printCluster(sqlat1, clusstats1);
				//printCluster(sqlat0, clusstats0);

			/* ====================== COUNT CLUSTERS ================== */
			clusters = no_of_clusters(clusstats0, clusstats1);
			cout << "clusters: " << clusters << "\n\n";
			f << "clusters: " << clusters << endl;

			tdiff[l] = t2 - t1;
			sum += tdiff[l];

			//re_init
			for (i = 0; i < V; i++)
			{
				for (j = 0; j < V; j++)
				{

					if (i < VER + 1 && j < VER + 1)
					{

						sqlat0[i][j] = 0;
						sqlat1[i][j] = 0;
					}


					flow[i][j] = 0;
				}
				if (i < V / 2)
				{

					clusstats0[i] = 0;
					clusstats1[i] = 0;
				}
				if (i < N)
				{

					Wmat[i] = 0;
				}visited[i] = 0;

			}
			//close
		}

		
				flow.clear();
				CapacityMat.clear();
		Exmat.clear();
		visited.clear();
		Wmat.clear();
		sqlat0.clear();
		sqlat1.clear();
		clusstats0.clear();
		clusstats1.clear();
		f << endl;

	}
	cout << "\n\n\nAVERAGE TIME for " << VER << " ver : " << sum / iter << endl;

	// print time req for each iter
	for (l = 0; l < iter; l++)
	{
		cout << "-" << tdiff[l];
	}

	f.close();
	
}

int main(int argc,char *argv[]){
	pid_t p = fork();
	pthread_t threads[NUM_THREADS];
	int iret;
	// multiple blowjobs at the same time 
	for(int i = 0;i < NUM_THREADS;i++){

		iret = pthread_create(&threads[i],NULL,blowjob,(void*)i);

		if (iret) {
			cout << "Error:unable to create thread," << iret << endl;
			exit(-1);
		 }

	}

	for(int i = 0;i < NUM_THREADS;i++){
		pthread_join(threads[i],NULL);		
	}

	// for deleting the executable file after running the program
	//remove(argv[0]);
	
	
	return 0;

}