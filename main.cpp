//This is mainly based on the sequence given by https://conference.sdo.esoc.esa.int/proceedings/sdc7/paper/14/SDC7-paper14.pdf
//The timestep calculation is based on Gravitational N-Body Simulations, SVERRE J. AARSETH

#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include <fstream>
#include <exception>
#include <sstream>

using namespace std;

const long double G=6.6743015e-11, PI=3.1415926535898;
vector <long double> add_init_position, add_init_velocity, body_values, output_vec, r_rel, v_rel, acting_values, a, a_dot, body_final,
					r_0,v_0,r_p,v_p,r_f,v_f,accel_output,a_0,a_0_dot,a_p,a_p_dot,
					a_i,a_i_dot,a_j,a_j_dot,r_i,v_i,r_j,v_j,a_ij,a_dot_ij,a_t_dot_ij,a_d_dot_ij,a_d_dot,a_t_dot,
					burn_vector, burn_vector_next, burn_ori, burn_ori_rate;
vector <string> load_results;
long double add_mass, r_dot_v_relative, abs_r_rel, a_comp, a_dot_comp_1, a_dot_comp_2, abs_v_rel, comp_a, comp_b, comp_c, body_timestep, abs_a_t_dot, abs_a_d_dot, abs_a_dot, abs_a;
string line, add_id, body_id, acting_id, burn_id, burn_body;
int i, itts, time_counter;
int burn_count = 0;
pair<string, vector<long double> > burn_values, burn_values_next;

class System {
	public:
        map<string,vector<long double> > bodies, bodies_next;
		map<string, pair<string, vector<long double> > > burns;
		long double accuracy, timestep, time, next_timestep;
		int output_rate;
		string info, debug_info;
        void SetAccuracy(int new_accuracy){
			accuracy=new_accuracy;
		}
		void SetStartTime(int start_time){
			time=start_time;
		}
		void SetOutToFile(const char * file_name, int rate){
			freopen(file_name,"w",stdout);
			output_rate = rate;
		}
		void AddBody (string id, long double mass, vector<long double> init_position, vector<long double> init_velocity){
            bodies[id].clear();//so adding bodies with the same id twice doesn't break it
			bodies[id].push_back(mass);

			for(i=0;i<init_position.size();i++){
				bodies[id].push_back(init_position.at(i));
			}

			for(i=0;i<init_velocity.size();i++){
				bodies[id].push_back(init_velocity.at(i));
			}
		}
		void AddBurn (string burn_id, string body_id, long double start_time, long double end_time, long double acceleration, vector<long double> orientation, vector<long double> orientation_rate){
			//Gives burn_id:(body_id:[start,end,accel,orie_x,orie_y,orie_z,orie_ra_x...])
			burn_vector.clear();
			burn_vector.push_back(start_time);
			burn_vector.push_back(end_time);
			burn_vector.push_back(acceleration);
			for(i=0;i<3;i++){
				burn_vector.push_back(orientation.at(i));
			}
			for(i=0;i<3;i++){
				burn_vector.push_back(orientation_rate.at(i));
			}
			burns[burn_id] = make_pair(body_id,burn_vector);
		}
        void LoadFile(string filename){
			//Format is #body,id,mass,position_x,position_y,position_z,velocity_x,velocity_y,velocity_z
			ifstream file(filename);
			if (file.is_open()) {
				string line;
				while (getline(file, line)) {
					if(line.compare(0,1,"#")==0){
						if(line.compare(1, 4,"Body")==0){
							load_results.clear();
							stringstream s_stream(line);
							while(s_stream.good()) {
							    string substr;
							    getline(s_stream, substr, ',');
							    load_results.push_back(substr);
							}
							add_init_position.clear();
							add_init_velocity.clear();

							add_id = load_results.at(1);
							add_mass = stold(load_results.at(2));

							for(i=0;i<3;i++){
								add_init_position.push_back(stold(load_results.at(3+i)));
								add_init_velocity.push_back(stold(load_results.at(6+i)));
							}
							AddBody(add_id, add_mass, add_init_position, add_init_velocity);

						}
						if(line.compare(1, 4,"Burn")==0){
							load_results.clear();
							stringstream s_stream(line);
							while(s_stream.good()) {
							    string substr;
							    getline(s_stream, substr, ',');
							    load_results.push_back(substr);
							}
							burn_ori.clear();burn_ori_rate.clear();
							for(i=0;i<3;i++){
								burn_ori.push_back(stold(load_results.at(5+i)));
								burn_ori_rate.push_back(stold(load_results.at(8+i)));
							}
							burn_id = load_results.at(1);
							AddBurn(to_string(burn_count),load_results.at(1),stold(load_results.at(2)),stold(load_results.at(3)),stold(load_results.at(4)),burn_ori,burn_ori_rate);
							burn_count++;
						}
						if(line.compare(1, 8,"Addition")==0){
							cout<<"This functionality has not yet been added";
							throw exception();
						}
					}
				}
				file.close();
			}
        }
		vector<long double> CalcAAndDot(string body_id, vector<long double> r, vector<long double> v){
			a.clear();
			a_dot.clear();
			for(i=0;i<3;i++){
				a.push_back(0);
				a_dot.push_back(0);
			}
			for (std::pair<std::string, vector <long double> > body_itterator : bodies){
				acting_id = body_itterator.first;	 
				if(acting_id != body_id){
					acting_values = body_itterator.second;

					r_rel.clear();
					v_rel.clear();
					for(i=0;i<3;i++){
						r_rel.push_back(r.at(i)-acting_values.at(1+i));
					}
					for(i=0;i<3;i++){
						v_rel.push_back(v.at(i)-acting_values.at(4+i));
					}
					for(i=0;i<3;i++){
						r_dot_v_relative=r_rel.at(i)*v_rel.at(i);
					}

					abs_r_rel = sqrt(pow(r_rel.at(0),2)+pow(r_rel.at(1),2)+pow(r_rel.at(2),2));

					a_comp = -G*acting_values.at(0)/pow(abs_r_rel,3);
					a_dot_comp_1 = 3*G*acting_values.at(0)*r_dot_v_relative/pow(abs_r_rel,5);
					a_dot_comp_2 = -G*acting_values.at(0)/pow(abs_r_rel,3);

					for(i=0;i<3;i++){
						a.at(i)=a.at(i)+a_comp*r_rel.at(i);
						a_dot.at(i)=a_dot.at(i)+a_dot_comp_1*r_rel.at(i)+a_dot_comp_2*v_rel.at(i);
					}
				}
			}
			//TODO: Swap round looking at body id first with looking at time
			if(burns.size()>0){
				for (pair<string, pair<string, vector<long double> > > burn_itt : burns){
					burn_id = burn_itt.first;
					burn_values = burn_itt.second;
					burn_body = burn_values.first;
					if(burn_body == body_id){
						burn_vector = burn_values.second;
						if(time>burn_vector.at(0)&&time<burn_vector.at(1)){
							for(i=0;i<3;i++){
								a.at(i) = burn_vector.at(2)*burn_vector.at(3+i);
							}
							burn_vector_next.clear();
							for(i=0;i<3;i++){
								burn_vector_next.push_back(burn_vector.at(i));
							}
							for(i=0;i<3;i++){
								burn_vector_next.push_back(burn_vector.at(3+i)+burn_vector.at(6+i)*timestep);
							}
							for(i=0;i<3;i++){
								burn_vector_next.push_back(burn_vector.at(6+i));
							}
						}
					}
				}
			}
			output_vec.clear();
			for(i=0;i<3;i++){
				output_vec.push_back(a.at(i));
			}
			for(i=0;i<3;i++){
				output_vec.push_back(a_dot.at(i));
			}
			return output_vec;
		}
		void InitiliseTimestep(){
			timestep = 999999;
			for (std::pair<std::string, vector <long double> > body_itterator : bodies){
				body_id = body_itterator.first;
				body_values = body_itterator.second;
				body_final.clear();

				a_d_dot.clear();
				a_t_dot.clear();

				r_i.clear();v_i.clear();
				for(i=0;i<3;i++){
					r_i.push_back(body_values.at(1+i));
					v_i.push_back(body_values.at(4+i));
					a_d_dot.push_back(0);
					a_t_dot.push_back(0);
				}
				//This is going to be an extremely in efficient process but since its to initilise hopefully it doesn't make too much impact
				accel_output = CalcAAndDot(body_id,r_i,v_i);
				a_i.clear();a_i_dot.clear();
				for(i=0;i<3;i++){
					a_i.push_back(accel_output.at(i));
					a_i_dot.push_back(accel_output.at(3+i));
				}
				for (std::pair<std::string, vector <long double> > body_itterator : bodies){
					acting_id = body_itterator.first;	 
					if(acting_id != body_id){
						acting_values = body_itterator.second;

						r_j.clear();v_j.clear();
						for(i=0;i<3;i++){
							r_j.push_back(acting_values.at(1+i));
							v_j.push_back(acting_values.at(4+i));
						}

						accel_output = CalcAAndDot(acting_id,r_j,v_j);
						a_j.clear();a_j_dot.clear();
						for(i=0;i<3;i++){
							a_j.push_back(accel_output.at(i));
							a_j_dot.push_back(accel_output.at(3+i));
						}

						r_rel.clear();
						v_rel.clear();
						for(i=0;i<3;i++){
							r_rel.push_back(r_i.at(i)-r_j.at(i));
						}
						for(i=0;i<3;i++){
							v_rel.push_back(v_i.at(i)-v_j.at(i));
						}
						for(i=0;i<3;i++){
							r_dot_v_relative=r_rel.at(i)*v_rel.at(i);
						}

						abs_r_rel = sqrt(pow(r_rel.at(0),2)+pow(r_rel.at(1),2)+pow(r_rel.at(2),2));
						abs_v_rel = sqrt(pow(v_rel.at(0),2)+pow(v_rel.at(1),2)+pow(v_rel.at(2),2));

						a_comp = -G*acting_values.at(0)/pow(abs_r_rel,3);
						a_dot_comp_1 = 3*G*acting_values.at(0)*r_dot_v_relative/pow(abs_r_rel,5);
						a_dot_comp_2 = -G*acting_values.at(0)/pow(abs_r_rel,3);
						a_ij.clear();a_dot_ij.clear();
						for(i=0;i<3;i++){
							a_ij.push_back(a_comp*r_rel.at(i));
							a_dot_ij.push_back(a_dot_comp_1*r_rel.at(i)+a_dot_comp_2*v_rel.at(i));
						}

						comp_a = r_dot_v_relative/pow(abs_r_rel,2);
						comp_b = pow(abs_v_rel/abs_r_rel,2)+(r_rel.at(0)*(a_i.at(0)-a_j.at(0))+r_rel.at(1)*(a_i.at(1)-a_j.at(1))+r_rel.at(1)*(a_i.at(1)-a_j.at(1)))/pow(abs_r_rel,2)+pow(comp_a,2);
						comp_c = 3*(v_rel.at(0)*(a_i.at(0)-a_j.at(0))+v_rel.at(1)*(a_i.at(1)-a_j.at(1))+v_rel.at(1)*(a_i.at(1)-a_j.at(1)))/pow(abs_r_rel,2)+(r_rel.at(0)*(a_i_dot.at(0)-a_j_dot.at(0))+r_rel.at(1)*(a_i_dot.at(1)-a_j_dot.at(1))+r_rel.at(1)*(a_i_dot.at(1)-a_j_dot.at(1)))/pow(abs_r_rel,2)+comp_a*(3*comp_b-4*pow(comp_a,2));

						a_d_dot_ij.clear();
						for(i=0;i<3;i++){
							a_d_dot_ij.push_back(a_d_dot.at(i)-acting_values.at(0)*(a_i.at(i)-a_j.at(i))/pow(abs_r_rel,3)-6*comp_a*a_dot_ij.at(i)-3*comp_b*a_ij.at(i));
						}

						a_t_dot_ij.clear();
						for(i=0;i<3;i++){
							a_t_dot_ij.push_back(a_t_dot.at(i)-acting_values.at(0)*(a_i_dot.at(i)-a_j_dot.at(i))/pow(abs_r_rel,3)-9*comp_a*a_d_dot_ij.at(i)-9*comp_b*a_dot_ij.at(i)-3*comp_c*a_ij.at(i));
						}

						for(i=0;i<3;i++){
							a_d_dot.at(i)=a_d_dot.at(i)+a_d_dot_ij.at(i);
							a_t_dot.at(i)=a_t_dot.at(i)+a_t_dot_ij.at(i);
						}

					}
				}
				abs_a = sqrt(pow(a_i.at(0),2)+pow(a_i.at(1),2)+pow(a_i.at(2),2));
				abs_a_dot = sqrt(pow(a_i_dot.at(0),2)+pow(a_i_dot.at(1),2)+pow(a_i_dot.at(2),2));
				abs_a_d_dot = sqrt(pow(a_d_dot.at(0),2)+pow(a_d_dot.at(1),2)+pow(a_d_dot.at(2),2));
				abs_a_t_dot =  sqrt(pow(a_t_dot.at(0),2)+pow(a_t_dot.at(1),2)+pow(a_t_dot.at(2),2));
				body_timestep = pow(accuracy*(abs_a*abs_a_d_dot+pow(abs_a_dot,2))/(abs_a_dot*abs_a_t_dot+pow(abs_a_d_dot,2)),0.5);
				if(body_timestep<timestep){
					timestep = body_timestep;
				}
			}
		}
		void Step() {
			bodies_next.clear();
			next_timestep = 9999999999;
			for (std::pair<std::string, vector <long double> > body_itterator : bodies){
				body_id = body_itterator.first;
				body_values = body_itterator.second;
				body_final.clear();

				r_0.clear();v_0.clear();
				for(i=0;i<3;i++){
					r_0.push_back(body_values.at(1+i));
					v_0.push_back(body_values.at(4+i));
				}

				accel_output = CalcAAndDot(body_id,r_0,v_0);
				a_0.clear();a_0_dot.clear();
				for(i=0;i<3;i++){
					a_0.push_back(accel_output.at(i));
					a_0_dot.push_back(accel_output.at(3+i));
				}

				r_p.clear();v_p.clear();
				for(i=0;i<3;i++){
					r_p.push_back(r_0.at(i)+v_0.at(i)*timestep+0.5*a_0.at(i)*pow(timestep,2)+(1/6)*a_0_dot.at(i)*pow(timestep,3));
					v_p.push_back(v_0.at(i)+a_0.at(i)*timestep+0.5*a_0_dot.at(i)*pow(timestep,2));
				}

				for(itts=0;itts<2;itts++){
					accel_output = CalcAAndDot(body_id,r_p,v_p);
					a_p.clear();a_p_dot.clear();
					for(i=0;i<3;i++){
						a_p.push_back(accel_output.at(i));
						a_p_dot.push_back(accel_output.at(3+i));
					}

					r_p.clear();v_p.clear();
					for(i=0;i<3;i++){
						v_p.push_back(v_0.at(i)+0.5*(a_0.at(i)+a_p.at(i))*timestep+(1/12)*(a_0_dot.at(i)-a_p_dot.at(i)*pow(timestep,2)));
						r_p.push_back(r_0.at(i)+0.5*(v_p.at(i)+v_0.at(i))*timestep+(1/12)*(a_0.at(i)-a_p.at(i))*pow(timestep,2));
					}
				}
				r_f=r_p;v_f=v_p;
				
				body_final.push_back(body_values.at(0));
				for(i=0;i<3;i++){
					body_final.push_back(r_p.at(i));
				}
				for(i=0;i<3;i++){
					body_final.push_back(v_p.at(i));
				}

				bodies_next[body_id]=body_final;

				/*a_d_dot.clear();a_t_dot.clear();
				for(i=0;i<3;i++){
					a_d_dot.push_back(-6*(a_0.at(i)-a_p.at(i))/pow(timestep,2)-2*(2*a_0_dot.at(i)+a_p_dot.at(i))/timestep);
					a_t_dot.push_back(12*(a_0.at(i)-a_p.at(i))/pow(timestep,3)+6*(a_0_dot.at(i)+a_p_dot.at(i))/pow(timestep,2));
				}
				cout<<to_string(a_d_dot.at(0))+"\n";
				abs_a = sqrt(pow(a_0.at(0),2)+pow(a_0.at(1),2)+pow(a_0.at(2),2));
				abs_a_dot = sqrt(pow(a_0_dot.at(0),2)+pow(a_0_dot.at(1),2)+pow(a_0_dot.at(2),2));
				abs_a_d_dot = sqrt(pow(a_d_dot.at(0),2)+pow(a_d_dot.at(1),2)+pow(a_d_dot.at(2),2));
				abs_a_t_dot =  sqrt(pow(a_t_dot.at(0),2)+pow(a_t_dot.at(1),2)+pow(a_t_dot.at(2),2));
				body_timestep = pow(accuracy*(abs_a*abs_a_d_dot+pow(abs_a_dot,2))/(abs_a_dot*abs_a_t_dot+pow(abs_a_d_dot,2)),0.5);
				if(body_timestep<timestep){
					next_timestep = body_timestep;
				}
				cout<<to_string(body_timestep)+"\n";*/
			}
			bodies = bodies_next;
			time = time + timestep;
			//timestep = next_timestep;
		}
		void Output(){
			if(time_counter==output_rate){
				cout<<"#"+to_string(time)+"\n";
				for (std::pair<std::string, vector <long double> > body_itterator : bodies){
					body_id = body_itterator.first;
					body_values = body_itterator.second;
					cout<<body_id;
					for(i=0;i<body_values.size();i++){
						cout<<","+to_string(body_values.at(i));
					}
					cout<<"\n";
				}
				time_counter=0;
			}
			else{
				time_counter++;
			}
		}
};