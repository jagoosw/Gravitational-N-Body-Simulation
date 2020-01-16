#include "main.cpp"
long int steps, step, n,output_frequency,output_counter;
long double e, mu, period, sma;
vector <long double> r_pe,v_pe, e_pos, e_vel;
string name, fname;

int main(){
    System sys;
    for(i=0;i<3;i++){
        e_pos.push_back(0);
        e_vel.push_back(0);
    }
	sys.SetAccuracy(10);
    mu = G*5.97219E+24;
    sma=6571000;
    period = 2*PI*sqrt(pow(sma,3)/mu);
    output_counter = 0;
    for(e=0.1;e<1.0;e=e+0.2){
        step = 0;
        sys.SetStartTime(0);
        name = "e="+to_string(e);
        r_pe.clear(),v_pe.clear();
        r_pe.push_back((1-e)*sma);
        r_pe.push_back(0);
        r_pe.push_back(0);
        v_pe.push_back(0);
        v_pe.push_back(sqrt((1+e)*mu/((1-e)*sma)));
        v_pe.push_back(0);
        sys.bodies.clear();
        sys.AddBody(name, 0.0001, r_pe, v_pe);
        sys.AddBody("Earth", 5.97219E+24, e_pos, e_vel);
        fname = "long_term_test_e="+to_string(e)+".csv";
        char fname_c[fname.size() + 1];
        fname.copy(fname_c, fname.size() + 1);
        fname_c[fname.size()] = '\0';
        freopen(fname_c,"w",stdout);
        n = (2.302585093-4.55*e-2.08)/(-0.000136);//should get 10m accuracy
        steps = n*10000;
        output_frequency = n/100+n;
        sys.timestep = period/n;
        for(step=0;step<steps;step++){
            if(output_counter == output_frequency){
                cout<<to_string(sys.bodies[name].at(1))+","+to_string(sys.bodies[name].at(2))+","+to_string(sys.bodies[name].at(3))+"\n";
                output_counter=0;
            }
            else{
                output_counter++;
            }
            sys.debug_info = to_string(step);           
            sys.Step();
        }
    }
}
