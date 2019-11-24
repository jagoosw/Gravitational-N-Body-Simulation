#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <cstring>

using namespace std;

string filename, body_id, out_name;
vector <string> load_results, ids;
map<string,vector<string > > bodies;
vector<string> tmp_vec;
int i,n;


int main(){
    string start_file = "burn_test.start";
    string output_file = "burn_test.csv";
    ifstream infile(start_file);
	if (infile.is_open()) {
        string line;
        while (getline(infile, line)) {
            if(line.compare(0,1,"#")==0){
                if(line.compare(1, 4,"Body")==0){
                    load_results.clear();
                    stringstream s_stream(line);
                    while(s_stream.good()) {
                        string substr;
                        getline(s_stream, substr, ',');
                        load_results.push_back(substr);
                    }
                    ids.push_back(load_results.at(1));
                }
            }
        }
    }
    ifstream outfile(output_file);
    if (outfile.is_open()) {
        string line;
        while (getline(outfile, line)) {
            if(line.compare(0,8,"Timestep")!=0){
                if(line.compare(0,1,"#")!=0){
                    for (string body_id : ids){ 
                        if(line.compare(0,5,body_id.substr(0,5))==0){
                            load_results.clear();
							stringstream s_stream(line);
							while(s_stream.good()) {
							    string substr;
							    getline(s_stream, substr, ',');
							    load_results.push_back(substr);
							}
                            bodies[body_id].push_back(load_results.at(2)+","+load_results.at(3)+","+load_results.at(4));
                        }
                    }
                }
            }
        }
    }
    for (string body_id : ids){ 
        out_name = body_id+"_output.csv";
        char out_name_c[out_name.size() + 1];
        out_name.copy(out_name_c, out_name.size() + 1);
        out_name_c[out_name.size()] = '\0';
        freopen(out_name_c,"w",stdout);
        for(i=0;i<bodies[body_id].size();i++){
            cout<<bodies[body_id].at(i);
            cout<<"\n";
        }
    }
}