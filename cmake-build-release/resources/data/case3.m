function mpc = case3
%CASE3_CUSTOM  Custom 3-bus system with Slack, PV, PQ

mpc.version = '2';
mpc.baseMVA = 100; % Can be 1 or 100, doesn't change p.u. values

%% bus data
% bus_i  type  Pd   Qd   Gs  Bs  area  Vm    Va    baseKV  zone  Vmax  Vmin
mpc.bus = [
    1   3   0    0    0   0   1    1.0   36.87   230   1    1.1   0.9;   % Slack (0.8+j0.6)
    2   2   0    0    0   0   1    1.02   0      230   1    1.1   0.9;   % PV
    3   1   150  80   0   0   1    1.0    0      230   1    1.1   0.9;   % PQ (load)
];

%% generator data
% bus  Pg   Qg   Qmax  Qmin  Vg   mBase  status  Pmax  Pmin
mpc.gen = [
    1   0    0    999   -999  1.0  100    1      999   0;
    2   300  0    999   -999  1.02 100    1      999   0;
];

%% branch data
% fbus  tbus  r       x       b     rateA  rateB  rateC  ratio  angle  status  angmin  angmax
mpc.branch = [
    1     2    0.05    0.1     0     250    250    250    0      0      1      -360     360;
    1     3    0.025   0.03    0     250    250    250    0      0      1      -360     360;
    2     3    0.02    0.02    0     250    250    250    0      0      1      -360     360;
];

%% generator cost data
% 2  startup  shutdown  n  c2  c1  c0
mpc.gencost = [
    2  0   0   3   0.01  1   0;
    2  0   0   3   0.01  1   0;
];
