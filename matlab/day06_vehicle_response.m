%% Day 06 - Vehicle Longitudinal Response and Speed Feedback
% Passive Dual-Sensor Driver Alcohol Detection and Controlled Drivetrain Inhibit System
%
% PURPOSE
% -------
% Model the vehicle response after a confirmed alcohol event.
%
% The Arduino/Tinkercad layer produces the safety decision and progressively
% reduces allowed propulsion. This MATLAB model represents the physical plant:
%
%   driver demand -> safety limit -> drive force
%                               + controlled brake force
%                               -> vehicle dynamics -> speed feedback
%
% IMPORTANT
% ---------
% These parameters are simulation values for a portfolio project.
% They are NOT production vehicle calibration values.

clear;
clc;
close all;

%% 1. Simulation settings
dt = 0.01;                 % simulation time step [s]
tEnd = 25;                 % total simulation time [s]
t = 0:dt:tEnd;

%% 2. Simplified vehicle parameters
m = 1200;                  % vehicle mass [kg]
g = 9.81;                  % gravity [m/s^2]
rho = 1.225;               % air density [kg/m^3]
Cd = 0.30;                 % aerodynamic drag coefficient
A = 2.2;                   % frontal area [m^2]
Crr = 0.015;               % rolling resistance coefficient

maxDriveForce = 700;       % maximum modeled propulsion force [N]
maxBrakeForce = 1800;      % maximum controlled intervention brake force [N]

%% 3. Driver and intervention scenario
initialSpeed = 20;         % 20 m/s = 72 km/h
driverDemand = 0.65;       % 65 percent accelerator request

interventionTime = 5;      % alcohol intervention begins at t = 5 s
slowdownRampTime = 6;      % safety limit falls to zero over 6 s

%% 4. Preallocate arrays
n = numel(t);

driverCommand = driverDemand * ones(1,n);
interventionProgress = zeros(1,n);
safetyLimit = ones(1,n);
allowedCommand = zeros(1,n);

driveForce = zeros(1,n);
brakeForce = zeros(1,n);
rollingForce = zeros(1,n);
aeroForce = zeros(1,n);
acceleration = zeros(1,n);

speed = zeros(1,n);
speed(1) = initialSpeed;

lockout = false(1,n);

%% 5. Build the safety command profile
for k = 1:n
    if t(k) < interventionTime
        interventionProgress(k) = 0;
        safetyLimit(k) = 1;
    else
        interventionProgress(k) = ...
            min((t(k) - interventionTime) / slowdownRampTime, 1);

        % The safety limit begins at the driver's current request and
        % progressively falls to zero.
        safetyLimit(k) = ...
            driverDemand * (1 - interventionProgress(k));
    end

    allowedCommand(k) = min(driverCommand(k), safetyLimit(k));

    % Braking is introduced progressively as intervention advances.
    brakeForce(k) = maxBrakeForce * interventionProgress(k);
end

%% 6. Simulate vehicle longitudinal motion
stopThreshold = 0.10;      % [m/s] approximately 0.36 km/h
stopIndex = NaN;

for k = 1:n-1

    v = speed(k);

    if v > 0
        rollingForce(k) = Crr * m * g;
        aeroForce(k) = 0.5 * rho * Cd * A * v^2;
    else
        rollingForce(k) = 0;
        aeroForce(k) = 0;
    end

    driveForce(k) = maxDriveForce * allowedCommand(k);

    netForce = ...
        driveForce(k) ...
        - brakeForce(k) ...
        - rollingForce(k) ...
        - aeroForce(k);

    acceleration(k) = netForce / m;

    nextSpeed = v + acceleration(k) * dt;

    % A longitudinal speed model must not produce reverse motion simply
    % because braking remains active after the vehicle has stopped.
    speed(k+1) = max(nextSpeed, 0);

    if isnan(stopIndex) && ...
       t(k) >= interventionTime && ...
       speed(k+1) <= stopThreshold

        stopIndex = k + 1;
    end
end

driveForce(end) = maxDriveForce * allowedCommand(end);

if speed(end) > 0
    rollingForce(end) = Crr * m * g;
    aeroForce(end) = 0.5 * rho * Cd * A * speed(end)^2;
end

%% 7. Generate speed-feedback lockout signal
if ~isnan(stopIndex)
    lockout(stopIndex:end) = true;
end

speedKmh = speed * 3.6;

%% 8. Report results
fprintf('\nDAY 06 VEHICLE RESPONSE RESULTS\n');
fprintf('--------------------------------\n');
fprintf('Initial speed: %.1f km/h\n', initialSpeed * 3.6);
fprintf('Intervention begins: %.1f s\n', interventionTime);
fprintf('Safety propulsion limit reaches zero: %.1f s\n', ...
    interventionTime + slowdownRampTime);

if ~isnan(stopIndex)
    fprintf('Simulated vehicle reaches stop threshold: %.2f s\n', ...
        t(stopIndex));
    fprintf('Stopping time after intervention: %.2f s\n', ...
        t(stopIndex) - interventionTime);
else
    fprintf('Vehicle did not reach the stop threshold within %.1f s.\n', tEnd);
end

%% 9. Save CSV results
results = table( ...
    t(:), ...
    driverCommand(:), ...
    safetyLimit(:), ...
    allowedCommand(:), ...
    brakeForce(:), ...
    speed(:), ...
    speedKmh(:), ...
    lockout(:), ...
    'VariableNames', { ...
        'Time_s', ...
        'DriverDemand', ...
        'SafetyLimit', ...
        'AllowedCommand', ...
        'BrakeForce_N', ...
        'Speed_mps', ...
        'Speed_kmh', ...
        'Lockout'});

if ~exist(fullfile('results','day06'), 'dir')
    mkdir(fullfile('results','day06'));
end

writetable(results, fullfile('results','day06','day06_vehicle_response.csv'));

%% 10. Plot 1 - Vehicle speed
figure;
plot(t, speedKmh, 'LineWidth', 1.5);
grid on;
xlabel('Time (s)');
ylabel('Vehicle Speed (km/h)');
title('Day 06 - Simulated Vehicle Speed Response');
xline(interventionTime, '--', 'Alcohol intervention begins');

if ~isnan(stopIndex)
    xline(t(stopIndex), '--', 'Speed feedback = stopped');
end

saveas(gcf, fullfile('results','day06','vehicle_speed_response.png'));

%% 11. Plot 2 - Driver request vs safety-limited propulsion
figure;
plot(t, driverCommand * 100, 'LineWidth', 1.5);
hold on;
plot(t, safetyLimit * 100, 'LineWidth', 1.5);
plot(t, allowedCommand * 100, 'LineWidth', 1.5);
grid on;
xlabel('Time (s)');
ylabel('Command (%)');
title('Day 06 - Driver Demand and Safety Arbitration');
legend('Driver Demand','Safety Limit','Allowed Command','Location','best');
saveas(gcf, fullfile('results','day06','safety_arbitration.png'));

%% 12. Plot 3 - Controlled braking force
figure;
plot(t, brakeForce, 'LineWidth', 1.5);
grid on;
xlabel('Time (s)');
ylabel('Brake Force (N)');
title('Day 06 - Controlled Intervention Brake Request');
saveas(gcf, fullfile('results','day06','controlled_braking.png'));

disp(' ');
disp('CSV and plots saved under results/day06/.');
