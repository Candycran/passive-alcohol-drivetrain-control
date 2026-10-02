%% Day 07 - Validate Day 06 Vehicle Results

clear;
clc;

%% Find the project root automatically

% Get the full location of this script
thisScript = mfilename('fullpath');

% Folder containing this script:
% ...\passive-driver-alcohol-drivetrain-system\matlab
scriptFolder = fileparts(thisScript);

% Parent folder is the project root:
% ...\passive-driver-alcohol-drivetrain-system
projectRoot = fileparts(scriptFolder);

%% Locate the Day 06 CSV

filePath = fullfile( ...
    projectRoot, ...
    'results', ...
    'day06', ...
    'day06_vehicle_response.csv' ...
);

fprintf('Looking for Day 06 results at:\n%s\n\n', filePath);

%% Confirm that the file exists

if ~isfile(filePath)
    error( ...
        'Day 06 CSV was not found. Expected file:\n%s', ...
        filePath ...
    );
end

%% Read the CSV

T = readtable(filePath);

%% Check required columns

requiredColumns = {
    'Time_s'
    'AllowedCommand'
    'Speed_mps'
    'Lockout'
};

for i = 1:length(requiredColumns)

    if ~ismember( ...
            requiredColumns{i}, ...
            T.Properties.VariableNames)

        error( ...
            'Required column missing from CSV: %s', ...
            requiredColumns{i} ...
        );

    end

end

%% Find when propulsion reaches zero

zeroPropulsionIndex = find( ...
    T.AllowedCommand <= 1e-6, ...
    1, ...
    'first' ...
);

%% Find when actual simulated vehicle speed reaches stop threshold

stopIndex = find( ...
    T.Speed_mps <= 0.10 & T.Time_s >= 5, ...
    1, ...
    'first' ...
);

%% Display final verification results

fprintf('\n');
fprintf('DAY 07 FINAL VEHICLE VALIDATION\n');
fprintf('--------------------------------\n');

if isempty(zeroPropulsionIndex)

    fprintf('FAIL: Propulsion never reached zero.\n');

else

    zeroTime = T.Time_s(zeroPropulsionIndex);

    fprintf( ...
        'PASS: Propulsion reached zero at %.2f s.\n', ...
        zeroTime ...
    );

end


if isempty(stopIndex)

    fprintf( ...
        'FAIL: Vehicle never reached the stop threshold.\n' ...
    );

else

    stopTime = T.Time_s(stopIndex);

    fprintf( ...
        'PASS: Vehicle reached stop threshold at %.2f s.\n', ...
        stopTime ...
    );

end


%% Verify that physical stopping happened AFTER propulsion reached zero

if ~isempty(zeroPropulsionIndex) && ~isempty(stopIndex)

    zeroTime = T.Time_s(zeroPropulsionIndex);
    stopTime = T.Time_s(stopIndex);

    if stopTime > zeroTime

        fprintf( ...
            ['PASS: Vehicle remained in motion after propulsion ', ...
             'reached zero.\n'] ...
        );

        fprintf( ...
            ['This confirms that independent vehicle-speed ', ...
             'feedback is necessary.\n'] ...
        );

        fprintf( ...
            'Time between zero propulsion and physical stop: %.2f s.\n', ...
            stopTime - zeroTime ...
        );

    else

        fprintf( ...
            ['CHECK: Vehicle stop occurred at or before zero ', ...
             'propulsion. Review the model settings.\n'] ...
        );

    end

end


%% Verify lockout feedback

lockoutIndex = find(T.Lockout == 1, 1, 'first');

if isempty(lockoutIndex)

    fprintf( ...
        'FAIL: Lockout feedback never became active.\n' ...
    );

else

    fprintf( ...
        'PASS: Lockout feedback became active at %.2f s.\n', ...
        T.Time_s(lockoutIndex) ...
    );

end

fprintf('--------------------------------\n');
fprintf('Day 07 validation complete.\n');