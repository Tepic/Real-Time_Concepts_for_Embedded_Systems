clear all
close all
clc

result = importdata('out.txt');

ts = (result.data(2:11))';
te = (result.data(12:21))';

deadlines = [2 2 4 10 15 40 15 36 0 40; 6 8 8 20 30 50 25 60 100 51];
scheduler = [ts; te];
         
tasks = [0:9; 0:9];

figure
plot(scheduler,tasks,'LineWidth',2);
hold all
plot(deadlines,tasks-0.1,'LineWidth',2);
hold all
plot(scheduler,[linspace(-1,-1,length(scheduler));linspace(-1,-1,length(scheduler))],'LineWidth',4);

grid on
set(gca, 'xtick', [0:2:100]);
set(gca, 'ytick', [-1:1:10]);
ylim([-1.5 9.5]);
xlabel('Time clock [tick]');
ylabel('Tasks 0..9 -> (T(-1) = final scheduler)');
title('SCHEDULER (Period = 100)');
