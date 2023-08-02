#include <bits/stdc++.h>
#include <sys/time.h>
using namespace std;

int NUM_HOSTS;
#define DEBUG 1

map<string, vector<long double> > allData;
vector< vector<long double> > bandwidth;
vector< vector<long double> > latency;
vector< vector<long double> > topology;
vector<string> hostnames;

map<string, int> node_attribute_direction;
map<string, long double> node_attribute_weight;
long double network_attribute_weight[2] = {0.3,0.7};
long double total_load_relative_weight[2] = {0.2,0.8};


string homedir = "/users/btech/tusharag/";

void getLiveHosts(string filename){
	ifstream hostfile(filename);
	string hostname;
	while(hostfile >> hostname){
		cout<<hostname<<" ";
		hostnames.push_back(hostname);
	}
	NUM_HOSTS = hostnames.size();
	cout<<endl<<"Number of hosts: "<<NUM_HOSTS<<endl;
}

int read_nodedata(string hostname, FILE* logfile){
	string datafile = homedir + "/.eagle/" + hostname + "/nodeinfo.txt";
  	ifstream myfile (datafile);
	if(myfile.is_open())
	{
		string line;
		getline (myfile,line);
		if(line=="") return -1;
		vector<string> token;
		istringstream iss(line);
		for(string s; iss >> s; )
			token.push_back(s);
		if(token.size() == 20){
			string field[20] = {"node", "cpucount" , "corecount", "cpufreqmin" , 
				"cpufreqcurrent" , "cpufreqmax" , "load_1", "load_5", "load_15","band_10", "band_50", "band_150", "util_10", 
				"util_50", "util_150", "memory", "memory_10", "memory_50", "memory_150",  "nodeusers"};
			allData["node"].push_back( stoi(token[0].substr(5,token[0].length())));
			for(int i=1;i<20;i++){
				allData[field[i]].push_back( stold(token[i]) );
			}
		}else
		{
			return -1;
		}
		
	} else{
		cout << "Unable to open node data file for " << hostname << endl;
		return -1; 
	}
	return 0;
}

void read_latency(vector<string> hostname, FILE* logfile){
	// string datafile = homedir + "/.eagle/" + hostname + "/latency.txt";
	string datafile = homedir + "/.eagle/lt.txt";
  	ifstream myfile (datafile);
	if(myfile.is_open())
	{
		string line;
		while ( getline (myfile,line) )
		{
			vector<string> token;
			istringstream iss(line);
			for(string s; iss >> s; )
    			token.push_back(s);
			if(token.size() == 3){
				int u = stoi(token[0].substr(5,token[0].length()-5));
				int v = stoi(token[1].substr(5,token[1].length()-5));
				int l = stold(token[2]);

				int uidx = find(allData["node"].begin(), allData["node"].end(), u) - allData["node"].begin();
				int vidx = find(allData["node"].begin(), allData["node"].end(), v) - allData["node"].begin();
				if (uidx!=NUM_HOSTS and vidx!=NUM_HOSTS){
					latency[uidx][vidx] = l;
					latency[vidx][uidx] = l;
				}
				if(uidx==vidx)
					latency[uidx][vidx] = 0;
			}
		}
	} else{
		cout << "Unable to open latency file" << endl; 
	}
}

void read_bw(vector<string> hostname, FILE* logfile){
	string datafile = homedir + "/.eagle/bw.txt";
  	ifstream myfile (datafile);
	int n = 0;
	if(myfile.is_open())
	{
		string line;
		while ( getline (myfile,line) )
		{
			vector<string> token;
			istringstream iss(line);
			for(string s; iss >> s; )
    			token.push_back(s);
			if(token.size() == 3){
				int u = stoi(token[0].substr(5,token[0].length()-5));
				int v = stoi(token[1].substr(5,token[1].length()-5));
				int l = stold(token[2]);

				int uidx = find(allData["node"].begin(), allData["node"].end(), u) - allData["node"].begin();
				int vidx = find(allData["node"].begin(), allData["node"].end(), v) - allData["node"].begin();
				if (uidx!=NUM_HOSTS and vidx!=NUM_HOSTS){
					n = n + 1;
					bandwidth[uidx][vidx] = l;
					bandwidth[vidx][uidx] = l;
				}
				if(uidx==vidx)
					bandwidth[uidx][vidx] = 0;
				}
		}
	} else{
		cout << "Unable to open bandwidth file"<< endl; 
	}
}


void read_topology( FILE* logfile){
	string datafile = homedir + "/.eagle/topology.txt";
  	ifstream myfile (datafile);
	int n = 0;
	if(myfile.is_open())
	{
		string line;
		while ( getline (myfile,line) )
		{
			vector<string> token;
			istringstream iss(line);
			for(string s; iss >> s; )
    			token.push_back(s);
			if(token.size() == 3){
				int u = stoi(token[0] );
				int v = stoi(token[1] );
				int l = stold(token[2]);
                
				int uidx = find(allData["node"].begin(), allData["node"].end(), u) - allData["node"].begin();
				int vidx = find(allData["node"].begin(), allData["node"].end(), v) - allData["node"].begin();
            

				if (uidx!=NUM_HOSTS and vidx!=NUM_HOSTS){
					n = n + 1;
					topology[uidx][vidx] = l;
				}
				if(uidx==vidx and uidx!=NUM_HOSTS and vidx!=NUM_HOSTS)
					topology[uidx][vidx] = 0;
                
			}
            
		}
	} else{
		cout << "Unable to open topology file " << endl; 
	}
    cout << "done" << endl;
}


void read_data(FILE *logfile){
    // Initialize bandwidth
	// vector< vector<long double> > > bandwidth(NUM_HOSTS, vector<long double> (NUM_HOSTS, 0))
	for(int i=0;i<NUM_HOSTS;i++){
		vector<long double> tmp;
		for(int j=0;j<NUM_HOSTS;j++){
			tmp.push_back(0);
		}
		bandwidth.push_back(tmp);
	}

	// Initialize latency
	for(int i=0;i<NUM_HOSTS;i++){
		vector<long double> tmp;
		for(int j=0;j<NUM_HOSTS;j++){
			tmp.push_back(0);
		}
		latency.push_back(tmp);
	}

    // Initialize topology
	for(int i=0;i<NUM_HOSTS;i++){
		vector<long double> tmp;
		for(int j=0;j<NUM_HOSTS;j++){
			tmp.push_back(0);
		}
		topology.push_back(tmp);
	}

	for(int idx=0; idx<NUM_HOSTS; idx++){
		int rc = read_nodedata(hostnames[idx], logfile);
		if(rc==-1){
			hostnames.erase(idx + hostnames.begin());
			idx--;
			NUM_HOSTS--;
		}
	}

	read_latency(hostnames, logfile);
	read_bw(hostnames, logfile);

    read_topology(logfile);
}


void set_node_attribute_direction(){
    node_attribute_direction.insert({"cpucount", -1});
	node_attribute_direction.insert({"corecount", -1});

    node_attribute_direction.insert({"cpufreqmax", -1});
	node_attribute_direction.insert({"cpufreqcurrent", -1});
	node_attribute_direction.insert({"cpufreqmin", -1});


	node_attribute_direction.insert({"load_1", 1});
	node_attribute_direction.insert({"load_5", 1});
	node_attribute_direction.insert({"load_15", 1});

	node_attribute_direction.insert({"band_10", 1});
	node_attribute_direction.insert({"band_50", 1});
	node_attribute_direction.insert({"band_150", 1});

	node_attribute_direction.insert({"util_10", 1});
	node_attribute_direction.insert({"util_50", 1});
    node_attribute_direction.insert({"util_150", 1});


	node_attribute_direction.insert({"memory", -1});
	node_attribute_direction.insert({"memory_10", -1});
	node_attribute_direction.insert({"memory_50", -1});
	node_attribute_direction.insert({"memory_150", -1});

	node_attribute_direction.insert({"nodeusers",-1});

}

void set_node_attributes_weight(){

	// node_attribute_weight.insert({"cpucount", 2});
	// weights.insert({"cpufreqmax", 2});
	// node_attribute_weight.insert({"corecount", 2});

	node_attribute_weight.insert({"load_1", 4});
	node_attribute_weight.insert({"load_5", 4});
	// weights.insert({"load_15", -4});

	node_attribute_weight.insert({"band_10", 3});
	// weights.insert({"band_50", -7});
	// weights.insert({"band_150", -6});

	node_attribute_weight.insert({"util_10", 4});
	// weights.insert({"util_50", -4});

	// // weights.insert({"memory", 2});
	node_attribute_weight.insert({"memory_10", 2});
	// weights.insert({"nodeusers",-1});
}

void compute_process_count_and_divide_load_by_cpu_count(){
	for(int i=0;i<NUM_HOSTS;i++){
		long double loadAvg = ( 5*allData["load_1"][i] + 3*allData["load_5"][i] + 2*allData["load_15"][i]) / 10.0;
		int processCount = allData["cpucount"][i] - (int)fmod(loadAvg,allData["cpucount"][i]);
		allData["processcount"].push_back(processCount);

		allData["load_1"][i] = allData["load_1"][i] / allData["cpucount"][i];
		allData["load_5"][i] = allData["load_5"][i] / allData["cpucount"][i];
		allData["load_15"][i] = allData["load_15"][i] / allData["cpucount"][i];

	}
	return;
}

void make_data_unidirectional(){

    // bandwidth direction change
    long double max_bw = 0;
    for(int i=0;i<NUM_HOSTS;i++){
		for(int j=0;j<NUM_HOSTS;j++){
            if(bandwidth[i][j] > max_bw){
                max_bw = bandwidth[i][j];
            }
		}
	}
    max_bw = max_bw + 10;
    for(int i=0;i<NUM_HOSTS;i++){
		for(int j=0;j<NUM_HOSTS;j++){
            bandwidth[i][j] = max_bw - bandwidth[i][j];
            if(i==j){
                bandwidth[i][j] = 0;
            }
		}
	}

    // make node attributes unidirectinal

	for( map<string, int>::iterator it=node_attribute_direction.begin();it!=node_attribute_direction.end();it++){
		string field = it->first;
		if(node_attribute_direction[field]<0){
			long double max_val =  *max_element(allData[field].begin(), allData[field].end());
			for(int i=0;i< NUM_HOSTS ;i++){
				allData[field][i] = max_val - allData[field][i];
			}
        }
	}

    return;
}


void normalize_and_compute_power(long double comp_power[]){
	
    // normalize
    for( map<string, long double>::iterator it=node_attribute_weight.begin();it!=node_attribute_weight.end();it++){
		string field = it->first;
        long double field_sum = accumulate(allData[field].begin(), allData[field].end(), 0.0) / allData[field].size();
        if(field_sum==0) field_sum = 1; 
	    transform(allData[field].begin(), allData[field].end(), allData[field].begin(), bind2nd(divides<long double>(), field_sum ));
	}

    // compute_power
	for(int i=0;i<NUM_HOSTS;i++){
		for(map<string, long double>::iterator it =node_attribute_weight.begin();it!=node_attribute_weight.end();it++){
			string field = it->first;
			comp_power[i] += node_attribute_weight[field]*allData[field][i];
		}
	}

	if(DEBUG)
	{
		cout << "COMPUTE POWER OF EACH NODE W/o NORM" << endl;
		for(int idx=0;idx<NUM_HOSTS;idx++)
			cout<<comp_power[idx]<<" ";
		cout<<endl;
	}

    // normalize compute power
	transform(comp_power,comp_power+NUM_HOSTS , comp_power, bind2nd(divides<long double>(), accumulate(comp_power, comp_power+NUM_HOSTS, 0.0)));
	
	if(DEBUG)
	{
		cout << "COMPUTE POWER OF EACH NODE" << endl;
		for(int idx=0;idx<NUM_HOSTS;idx++)
			cout<<comp_power[idx]<<" ";
		cout<<endl;
	}
    return;
}

vector<int> FindMinStar(vector<int> G, vector<int> counts, int node, int n){
	
	vector<int> V_;
	V_.push_back(node);

	int size = counts[node];

	vector<long double > bw_array;
	vector<long double > lt_array;

	for(int i=0;i < NUM_HOSTS;i++){
		bw_array.push_back({bandwidth[node][i]});
		lt_array.push_back({latency[node][i]});
	}

	long double bw_sum =  accumulate(bw_array.begin(), bw_array.end(), 0.0);
	if(bw_sum==0) bw_sum = 1;
	transform(bw_array.begin(), bw_array.end(), bw_array.begin() , bind2nd(divides<long double>(), bw_sum));

	long double lt_sum =  accumulate(lt_array.begin(), lt_array.end(), 0.0);
	if(lt_sum==0) lt_sum = 1;
	transform(lt_array.begin(), lt_array.end(), lt_array.begin() , bind2nd(divides<long double>(), lt_sum));

	vector<pair<double,int> > netw_array;
	for(int i=0; i< NUM_HOSTS;i++){
		long double netw = network_attribute_weight[0]*lt_array[i] + network_attribute_weight[1]*bw_array[i];
		netw_array.push_back({netw, i});
	}
	sort(netw_array.begin(),netw_array.end());

    int i = 0;
	while(size<n and i<netw_array.size()){
		int node_index = netw_array[i].second;
        i = i + 1;
		if(find(G.begin(), G.end(), node_index) == G.end() or node_index==node)
			continue;
		V_.push_back(node_index);
		size = size + counts[node_index];
	}

	if(size<n){
		vector<int> tmp;
		return tmp;
	}
	return V_;
}

long double compute_Comp(vector<int> G, long double comp_power[]){
	long double power = 0;
	for(int i=0;i<G.size();i++){
		power += comp_power[G[i]];
	}
	return power / G.size();
}

long double compute_BwNetw(vector<int> G){
	long double power = 0;
	for(int i=0;i<G.size();i++){
		for(int j=0;j<G.size();j++){
			power += bandwidth[G[i]][G[j]];
		}
	}
	if(G.size()==1)
		return 0;
	power /= (G.size()*G.size());
	return power;
}

long double compute_LtNetw(vector<int> G){
	long double power = 0;
	for(int i=0;i<G.size();i++){
		for(int j=0;j<G.size();j++){
			power += latency[G[i]][G[j]];
		}
	}
	if(G.size()==1)
		return 0;
	power /= (G.size()*G.size());
	return power;
}

vector<int> FindBestStar(vector<int> G, vector<int> counts, int n, long double comp_power[]){
	
	vector<int> G_;
	long double comp[NUM_HOSTS] = {0};
	long double netw[NUM_HOSTS] = {0};
    long double bw[NUM_HOSTS] = {0};
    long double lt[NUM_HOSTS] = {0};
	long double total_load[NUM_HOSTS] = {0};

	// Calculate compute and network power
	for(auto it=G.begin();it!=G.end();it++){
		int node = int(*it);
		vector<int> G__ = FindMinStar(G, counts, node, n);
		if (G__.size()==0){
			continue;
		}
		comp[node] = compute_Comp(G__, comp_power);
		bw[node] = compute_BwNetw(G__);
        lt[node] = compute_LtNetw(G__);
	}

	// Normalize computation load of each star
	long double comp_sum = accumulate(comp, comp+NUM_HOSTS, 0.0);
	if(comp_sum==0) comp_sum = 1;
	transform(comp, comp + NUM_HOSTS, comp, bind2nd(divides<long double>(),comp_sum ));

    // Normalize bw load of each star
	long double band_sum =  accumulate(bw, bw+NUM_HOSTS, 0.0);
	if(band_sum==0) band_sum = 1;
	transform(bw, bw + NUM_HOSTS, bw, bind2nd(divides<long double>(), band_sum));
    
    // Normalize latency of each star
    long double lt_sum =  accumulate(lt, lt+NUM_HOSTS, 0.0);
	if(lt_sum==0) lt_sum = 1;
	transform(lt, lt + NUM_HOSTS, lt, bind2nd(divides<long double>(), lt_sum));

    // compute and normalize network load of each star
    for(int i=0;i<NUM_HOSTS;i++){
		netw[i] = network_attribute_weight[0]*(lt[i]) + network_attribute_weight[1]*bw[i];
	}
    long double netw_sum =  accumulate(netw, netw+NUM_HOSTS, 0.0);
	if(netw_sum==0) netw_sum = 1;
	transform(netw, netw + NUM_HOSTS, netw, bind2nd(divides<long double>(), netw_sum));

	// Compute total load
	int min_idx = 0;
	long double min_load = 10000000;
	for(int i=0;i<NUM_HOSTS;i++){
		total_load[i] = total_load_relative_weight[0]*(comp[i]) + total_load_relative_weight[1]*netw[i];
		if(total_load[i] < min_load){
			min_load = total_load[i];
			min_idx = i;
		}
	}

	if(DEBUG==1){
		cout<<"Computation, Network and Total load for each node star:\n"<<endl;
		for(int i=0;i<NUM_HOSTS;i++){
			cout<<allData["node"][i]<<" || "<<comp[i]<<" || "<<netw[i]<<" || "<<total_load[i]<<endl;
		}
		cout<<"Min Load:"<< min_load << " " << allData["node"][min_idx] <<endl;
	}

	G_ = FindMinStar(G, counts, min_idx, n);
	return G_;
}


int main(int argc, char **argv){
	if(argc<3){
		cout<<"Usage: <executable> <number of hosts> <ppn>"<<endl;
		exit(0);
	}
	homedir = getenv("HOME");
    FILE *logfile;
	logfile = fopen("allocator.log","w");

	int required_cores = atoi(argv[1]);
	int ppn  = atoi(argv[2]);

	// Get list of hosts
	getLiveHosts(homedir+"/.eagle/livehosts.txt");

    // read all required_data
    read_data(logfile);

    //Find Proccess count to be allocated on each node
	compute_process_count_and_divide_load_by_cpu_count();
	
    if(DEBUG){
		cout << "Process and Cpu Count\n ";
		for(int i=0;i< NUM_HOSTS; i++){
			cout << allData["processcount"][i] << "," << allData["cpucount"][i] << " ";
		}
		cout << endl;
	}
	if(ppn!=0){
		for(int i=0;i<NUM_HOSTS;i++){
			allData["processcount"][i] = ppn;
		}
	}

    set_node_attribute_direction();
    set_node_attributes_weight();

    // make all network and compute attribute unidirectional
    make_data_unidirectional();

	if(DEBUG){
		cout<<"Bandwidth Data :\n";
		for(int i=0;i<NUM_HOSTS;i++){
			cout << hostnames[i] << "   " ;
			for(int j=0;j<NUM_HOSTS;j++)
				cout<<bandwidth[i][j]<<" ";
			cout<<endl;
		}
		cout<<"Latency Data :\n";
		for(int i=0;i<NUM_HOSTS;i++){
			for(int j=0;j<NUM_HOSTS;j++)
				cout<<latency[i][j]<<" ";
			cout<<endl;
		}

        cout<<"Topology Data :\n";
		for(int i=0;i<NUM_HOSTS;i++){
			for(int j=0;j<NUM_HOSTS;j++)
				cout<<topology[i][j]<<" ";
			cout<<endl;
		}

		cout << "Node Data :\n";
		for(map<string, vector< long double> >::iterator it=allData.begin();it!=allData.end();it++){
			string key = it->first;
			cout << key << " ";
			for(int i=0;i<NUM_HOSTS;i++){
				cout << ":" << allData[key][i] << " ";
			}
			cout << endl;
		}
		cout << endl;
		
	}

	// time_t current_time;
	// current_time = time(NULL);
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);

	long double comp_power[NUM_HOSTS] = {0};
	normalize_and_compute_power(comp_power);

	
	vector<int> active_nodes;
	vector<int> core_count;

	for(int i=0;i<NUM_HOSTS;i++)
		core_count.push_back(int(allData["processcount"][i]));
	for(int i=0;i<NUM_HOSTS;i++)
		active_nodes.push_back(i);

	if(DEBUG){
		cout<<endl<<"Nodes"<<endl;
		for(int i=0;i<NUM_HOSTS;i++){
			cout<<allData["node"][i]<<" ";
		}
		cout<<endl;
		for(int i=0;i<NUM_HOSTS;i++){
			cout<<hostnames[i]<<" ";
		}
		cout<<endl;
	}



	vector<int> G = FindBestStar(active_nodes, core_count, required_cores, comp_power);

	
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	uint64_t delta = (end.tv_nsec - start.tv_nsec);

	cout << "Time"<< delta/1000000.0 <<"ms"<< endl;

	if(G.size()==0)
		cout<<"Not found\n";
	
	vector< pair<long double, int> > comp_power_with_index;
	for(int i=0;i< NUM_HOSTS;i++){
		comp_power_with_index.push_back({comp_power[i],i});
	}
	sort(comp_power_with_index.begin(), comp_power_with_index.end());
	
	FILE *hosts;
	hosts = fopen("hosts","w");
	if(DEBUG){
		for(auto it = G.begin(); it!=G.end();it++){
			cout<<allData["node"][*it]<<" "<<allData["processcount"][*it]<<endl;
			fprintf(hosts, "csews%d:%d\n", int(allData["node"][*it]), int(allData["processcount"][*it]));
		}
		cout<<endl;
	}

	fclose(hosts);

	FILE *comphosts;
	comphosts = fopen("comphosts","w");
	cout << "NODES ALLOCATION WITH MAX COMPUTE" << endl;
	if(DEBUG){
		for(int i=0; i< G.size(); i++){
			int idx = comp_power_with_index[i].second;
			cout << hostnames[idx] << endl;
			fprintf(comphosts, "%s\n",  (hostnames[idx]).c_str() );
		}
		cout<<endl;
	}

	fclose(comphosts);

	fclose(logfile);
	return 0;
}
