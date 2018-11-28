clear all
close all
clc

L    = [1/31  1/45    1/54  1/32];
E_b  = [2.5   3.375   2.25  2.5];
E_b2 = [8.25  12.625  6     8];

U = 27/42;
UA = sum(E_b.*L);
W0 = sum(L.*E_b2./2);

L_all = sum(L);

queing_time  = W0/( ((1-U)^2) *( 1-UA / (1-U) ) );
average_time = sum( L.*E_b )/(L_all*(1-U));
W = average_time + queing_time;

disp(['Periodic task CPU Utilazation ->              U  = ',num2str(U)]);
disp(['Aperiodic task CPU Utilazation ->             UA = ',num2str(UA)]);

disp(['Queing time: ', num2str(queing_time)]);
disp(['Average time: ', num2str(average_time)]);
disp(['Average response time of aperiodic tasks is:  W  = ', num2str(W)]);