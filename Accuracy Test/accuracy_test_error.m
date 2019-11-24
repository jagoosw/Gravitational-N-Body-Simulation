clear all;
close all;
n=10000;
e=0.0;
a=6471000;
counter = 1;
while e<0.75
    while n<50000
        if e == 0
            name = "~/Documents/Uni/Other/CV/Gravitational-N-Body-Simulation-/V01/accuracy_test_e=0.000000_calls_per_rev="+n+".csv"
        else
            name = "~/Documents/Uni/Other/CV/Gravitational-N-Body-Simulation-/V01/accuracy_test_e="+e+"00000_calls_per_rev="+n+".csv"
        end
        tmp = csvread(name);
        p_pe = tmp(1,:);
        %p_ae = tmp(2,:);
        tmp(1,:) = [];
        tmp(1,:) = [];
        ticker = 0;
        r = (tmp(:,4).^2+tmp(:,5).^2+tmp(:,6).^2).^0.5;
        v = (tmp(:,7).^2+tmp(:,8).^2+tmp(:,9).^2).^0.5;
        r = r-p_pe(1);
        v = v-p_pe(2);
        x = [1:1:length(r)];
        plot(x,r.^0.5)
        text(max(x),max(r.^0.5),0,"e="+e+",n="+n)
        [fit,error] = polyfit(x.',r.^0.5,1);
        [y,delta] = polyval(fit,x,error);
        error = mean(delta)/sqrt(length(delta));
        fits(1,counter) = e;
        fits(2,counter) = n;
        fits(3,counter) = abs(fit(1));
        fits(4,counter) = error;
        hold on
        n=n+10000;
        counter = counter + 1;
    end
    n=10000;
    e = e+0.1;
end
xlabel("Complete revolutions")
ylabel("Distance error/m")
hold off