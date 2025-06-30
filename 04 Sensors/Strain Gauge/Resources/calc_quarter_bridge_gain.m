%% 
clear all
clc

% Strain gauge specifications
gf = 2;  % gauge factor
emax = 30000e-6; % max strain   
rgauge = 350;    % resistance of the strain gauge

% PCB variables
vadc = 5;    % ADC logic voltage
vexc = 5;    % excitation voltage to the Wheatstone bridge


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Calculations 
rbc = rgauge;    % resistance of the bridge completion resistor

r_percent_change_max = emax*gf;
delta_r_max = rgauge*r_percent_change_max;
rgauge_max = rgauge+delta_r_max;
rgauge_min = rgauge-delta_r_max;

voffset_comp = abs(vexc*(rbc/(rbc+rgauge_min)-1/2));
voffset_tension = abs(vexc*(rbc/(rbc+rgauge_max)-1/2));
voffset_max = max(voffset_tension,voffset_comp);

raw_range = [2.5-voffset_tension, 2.5+voffset_comp];
G = 1/2*vadc/voffset_max;
signal_range = [G*(voffset_max)-2.5, G*(voffset_max)+2.5];

rgain = 49.4e3/(G-1);

disp("Solder " + num2str(rbc) + " Ohm to bridge completion resistors.")
disp("Solder " + num2str(rgain) + " Ohm to instrumentation amplifier gain resistor.")



