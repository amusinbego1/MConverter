function mpc = case3_abur_exposito
%CASE3_ABUR_EXPOSITO  3-bus example based on Abur & Exposito

mpc.version = '2';
mpc.baseMVA = 100;

%% bus data
% bus_i type Pd Qd Gs Bs area Vm Va baseKV zone Vmax Vmin
%% bus data
% bus_i type Pd Qd Gs Bs area Vm Va baseKV zone Vmax Vmin
mpc.bus = [
    1 3   0     0     0 0 1 1.006 0 230 1 1.1 0.9;   % Slack
    2 1   50.1  28.6  0 0 1 0.968 0 230 1 1.1 0.9;   % Load
    3 1   61.0  37.0  0 0 1 1.000 0 230 1 1.1 0.9;   % Implied from injections
];


%% generator data
% bus Pg Qg Qmax Qmin Vg mBase status Pmax Pmin
mpc.gen = [
    1  0 0 999 -999 1.006 100 1 250 10;
];

%% branch data
% fbus tbus r x b rateA rateB rateC ratio angle status angmin angmax
mpc.branch = [
    1 2 0 0.03333 0 250 250 250 0 0 1 -360 360;   % g=10, b=-30
    1 3 0 0.058   0 250 250 250 0 0 1 -360 360;   % g=6.8966, b=-17.2414
    2 3 0 0.0912  0 250 250 250 0 0 1 -360 360;   % g=4.1096, b=-10.9589
];

%% measurement data (uniform fields for all structs)
% Fields: type, fbus, tbus, bus, value, sigma
% Use NaN for unused fields

mpc.measurements = [
    struct('type', 'PF', 'fbus', 1, 'tbus', 2, 'bus', NaN, 'value', 0.888,  'sigma', 0.008),  % P_12
    struct('type', 'PF', 'fbus', 1, 'tbus', 3, 'bus', NaN, 'value', 1.173,  'sigma', 0.008),  % P_13
    struct('type', 'PI', 'fbus', NaN, 'tbus', NaN, 'bus', 2, 'value', -0.501, 'sigma', 0.01),   % P_2
    struct('type', 'QF', 'fbus', 1, 'tbus', 2, 'bus', NaN, 'value', 0.568,  'sigma', 0.008),  % Q_12
    struct('type', 'QF', 'fbus', 1, 'tbus', 3, 'bus', NaN, 'value', 0.663,  'sigma', 0.008),  % Q_13
    struct('type', 'QI', 'fbus', NaN, 'tbus', NaN, 'bus', 2, 'value', -0.286, 'sigma', 0.01),   % Q_2
    struct('type', 'VM', 'fbus', NaN, 'tbus', NaN, 'bus', 1, 'value', 1.006,  'sigma', 0.004),  % V_1
    struct('type', 'VM', 'fbus', NaN, 'tbus', NaN, 'bus', 2, 'value', 0.968,  'sigma', 0.004)   % V_2
];
end
