#include "main.cpp"
int current_step, tstep, n, ticker;
long double steps, end_time, sma, mu, e, period, eps_d, eps_v, d_pe, s_pe, d_ae, s_ae, d, v;
vector <long double> r_pe, v_pe, e_pos, e_vel;
string fname, name;
//Eccentricity of 0.9 doesn't work for some reason so will be excluded, problem seems to be with test script not main
int main(){
    System sys;
	sys.SetAccuracy(10);
	sys.SetStartTime(0);
    mu = G*5.97219E+24;
    period = 2*PI*sqrt(pow(sma,3)/mu);
    sma=6471000;
    //Want 3.5 revolutions total
    for(i=0;i<3;i++){
        e_pos.push_back(0);
        e_vel.push_back(0);
    }
    for(e=0;e<0.9;e=e+0.1){
        r_pe.clear(),v_pe.clear();
        r_pe.push_back((1-e)*sma);
        r_pe.push_back(0);
        r_pe.push_back(0);
        v_pe.push_back(0);
        v_pe.push_back(sqrt((1+e)*mu/((1-e)*sma)));
        v_pe.push_back(0);
        name = "e="+to_string(e);

        period = 2*PI*sqrt(pow(sma,3)/mu);

        d_pe = (1-e)*sma;
        s_pe = sqrt((1+e)*mu/((1-e)*sma));
        d_ae = (1+e)*sma;
        s_ae = -sqrt((1-e)*mu/((1+e)*sma));
        for(n=5000;n<30000;n=n+5000){
            sys.bodies.clear();
            sys.AddBody(name, 0.01, r_pe, v_pe);
            sys.AddBody("Earth", 5.97219E+24, e_pos, e_vel);
            sys.timestep=period/(2*n);
            steps = 200*period/sys.timestep;
            sys.info= to_string(steps);
            fname = "accuracy_test_e="+to_string(e)+"_calls_per_rev="+to_string(2*n)+".csv";
            char fname_c[fname.size() + 1];
            fname.copy(fname_c, fname.size() + 1);
            fname_c[fname.size()] = '\0';
            freopen(fname_c,"w",stdout);
            cout<<to_string(d_pe)+","+to_string(s_pe)+"\n";
            cout<<to_string(d_ae)+","+to_string(s_ae)+"\n";
            ticker = 0;
            //check at every half revolution which will occure every n steps
            for(current_step=0;current_step<steps;current_step++){
                //sys.info = to_string(steps)+" "+to_string(current_step)+" "+name+" "+to_string(n);
                if(current_step%(2*n)==0){
                    if(ticker==0){
                        ticker=0;
                        sys.info = sys.info+" "+to_string((sys.bodies[name].at(1)-d_pe))+" "+to_string((sys.bodies[name].at(2)))+" "+to_string((sys.bodies[name].at(3)))+" "+to_string((sys.bodies[name].at(4)))+" "+to_string((sys.bodies[name].at(5)))+" "+to_string((sys.bodies[name].at(6)));
                        //eps_d = -log10(sqrt(pow((sys.bodies[name].at(1)-d_pe),2)+pow((sys.bodies[name].at(2)),2)+pow((sys.bodies[name].at(3)),2)));
                        //eps_v = -log10(sqrt(pow((sys.bodies[name].at(3)),2)+pow((sys.bodies[name].at(4)-s_pe),2)+pow((sys.bodies[name].at(5)),2)));
                        eps_d=eps_v=0;
                        cout<<to_string(current_step)+","+to_string(eps_d)+","+to_string(eps_v)+","+to_string((sys.bodies[name].at(1)))+","+to_string((sys.bodies[name].at(2)))+","+to_string((sys.bodies[name].at(3)))+","+to_string((sys.bodies[name].at(4)))+","+to_string((sys.bodies[name].at(5)))+","+to_string((sys.bodies[name].at(6)))+"\n";
                    }
                    /*else{
                        if(ticker==1){
                            ticker=0;
                            eps_d = -log10(sqrt(pow((sys.bodies[name].at(1)-d_ae),2)+pow((sys.bodies[name].at(2)),2)+pow((sys.bodies[name].at(3)),2)));
                            eps_v = -log10(sqrt(pow((sys.bodies[name].at(3)),2)+pow((sys.bodies[name].at(4)-s_ae),2)+pow((sys.bodies[name].at(5)),2)));
                            cout<<to_string(current_step)+","+to_string(eps_d)+","+to_string(eps_v)+","+to_string((sys.bodies[name].at(1)))+","+to_string((sys.bodies[name].at(2)))+""+to_string((sys.bodies[name].at(3)))+","+to_string((sys.bodies[name].at(4)))+","+to_string((sys.bodies[name].at(5)))+","+to_string((sys.bodies[name].at(6)))+"\n";
                        }
                    }*/
                }
                //sys.Output();
                //cout<<"\n";
                //for(i=1;i<7;i++){
				//	cout<<to_string(sys.bodies[name].at(i))+",";
				//}
                sys.Step();
            }
        }
    }
}