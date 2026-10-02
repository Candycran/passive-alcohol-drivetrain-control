# Day 06 — Manual Simulink Build

## Objective

Build a one-dimensional longitudinal vehicle model that converts the safety controller's allowed propulsion command into a physical vehicle-speed response.

The model does **not** simulate a complete car. It represents only the longitudinal motion needed by this project.

The key equation is:

`m * a = F_drive - F_brake - F_rolling - F_aero`

or:

`a = (F_drive - F_brake - F_rolling - F_aero) / m`

The Integrator converts acceleration into vehicle speed.

## Parameters to use

- Vehicle mass `m = 1200 kg`
- Initial vehicle speed `20 m/s` (72 km/h)
- Driver demand `0.65`
- Intervention start `5 s`
- Slowdown ramp duration `6 s`
- Maximum drive force `700 N`
- Maximum controlled brake force `1800 N`
- Rolling resistance force approximately `176.58 N`
- Aerodynamic-force gain `0.40425`
- Stop threshold `0.10 m/s`

These are simulation parameters, not production vehicle calibration values.

## Part A — Create the model

1. Open MATLAB.
2. Type `simulink` in the Command Window and press Enter.
3. Choose **Blank Model**.
4. Save it immediately as:

`day06_vehicle_speed_feedback.slx`

5. Set the simulation Stop Time to `25`.

## Part B — Add the driver-demand block

Double-click an empty area and search for **Constant**.

Add a Constant block.

Rename it:

`Driver Demand`

Set its value to:

`0.65`

This represents a driver holding approximately 65% accelerator demand.

## Part C — Create intervention progress

Add a **Ramp** block.

Rename it:

`Intervention Progress Ramp`

Set:

- Slope = `1/6`
- Start time = `5`
- Initial output = `0`

The raw ramp begins at 5 seconds and increases by 1/6 each second.

Add a **Saturation** block after it.

Rename it:

`Limit Progress 0 to 1`

Set:

- Lower limit = `0`
- Upper limit = `1`

Now intervention progress behaves as:

- 0 before 5 s
- progressively 0 to 1 from 5 to 11 s
- 1 after 11 s

## Part D — Create the shrinking safety limit

We need:

`SafetyLimit = 0.65 * (1 - InterventionProgress)`

Add a **Gain** block after the progress signal.

Set Gain to:

`-0.65`

Add a **Bias** block after it.

Set Bias to:

`0.65`

Rename the Bias output conceptually as:

`Safety Limit`

At 5 s it is approximately 0.65.

At 11 s it reaches 0.

## Part E — Safety arbitration

Add a **MinMax** block.

Configure it for:

`min`

Give it two inputs:

1. Driver Demand
2. Safety Limit

The output is:

`Allowed Command`

This reproduces our Arduino rule:

`Allowed = min(Driver Request, Safety Limit)`

The driver can request less propulsion, but cannot exceed the intervention limit.

## Part F — Convert allowed command into drive force

Add a **Gain** block after Allowed Command.

Set:

`Gain = 700`

Rename:

`Drive Force`

This gives:

`F_drive = AllowedCommand * 700 N`

## Part G — Controlled brake force

Branch the Intervention Progress signal.

Connect it to a new Gain block.

Set:

`Gain = 1800`

Rename:

`Controlled Brake Force`

This means braking rises progressively as the intervention progresses.

At progress 0:

`Brake Force = 0 N`

At progress 1:

`Brake Force = 1800 N`

## Part H — Build aerodynamic drag

Later in the model, vehicle speed feeds back into the drag calculation.

Take vehicle speed and connect it to a **Math Function** block.

Configure the Math Function as:

`square`

Then connect that to a Gain block.

Set Gain to:

`0.40425`

The output represents:

`F_aero = 0.5 * rho * Cd * A * v^2`

for the chosen simulation parameters.

## Part I — Rolling resistance

Add another Constant block.

Set:

`176.58`

Rename:

`Rolling Resistance`

This represents:

`Crr * m * g`

for the chosen parameter set.

## Part J — Net force

Add a **Sum** block.

Configure its signs as:

`+---`

Connect:

1. Drive Force to the positive input
2. Controlled Brake Force to negative
3. Rolling Resistance to negative
4. Aerodynamic Drag to negative

The output is:

`Net Force`

## Part K — Convert force to acceleration

Add a Gain block.

Set:

`Gain = 1/1200`

Rename:

`1 / Vehicle Mass`

The output is acceleration in m/s^2.

## Part L — Integrate acceleration to speed

Add an **Integrator** block.

Open its parameters.

Set Initial condition:

`20`

Enable output limiting if your Simulink version exposes the option.

Set Lower saturation limit:

`0`

This prevents the simplified model from creating negative vehicle speed after stopping.

Rename the output signal:

`Vehicle Speed mps`

The Integrator is the heart of this physical model:

`acceleration -> integrate over time -> speed`

## Part M — Convert speed to km/h

Branch the speed signal into a Gain block.

Set:

`3.6`

Rename:

`mps to kmph`

This produces vehicle speed in km/h.

Connect this output to a **Scope**.

## Part N — Create speed feedback / stopped detection

Branch the speed signal before the 3.6 conversion.

Add a **Compare To Constant** block.

Configure:

- Operator: `<=`
- Constant value: `0.10`

Rename:

`Vehicle Stopped`

The block becomes true when the simulated speed is approximately zero.

This is the feedback signal our overall safety logic needs.

The final logic is conceptually:

`Lockout = Intervention Confirmed AND Vehicle Stopped`

For this Day 06 scenario the intervention is already assumed to have been confirmed at 5 seconds.

## Part O — Add scopes

Create separate Scope blocks for:

1. Vehicle Speed
2. Driver Demand / Safety Limit / Allowed Command
3. Intervention Brake Force
4. Vehicle Stopped boolean

Use a **Mux** only where you want multiple related signals on the same Scope.

## Expected behavior

From 0 to about 5 seconds:

- driver demand remains 65%;
- intervention progress is zero;
- controlled brake force is zero;
- vehicle continues moving normally.

After 5 seconds:

- intervention progress begins rising;
- safety limit begins falling;
- allowed propulsion begins falling;
- controlled brake force begins rising;
- vehicle speed decreases smoothly.

At approximately 11 seconds:

- allowed propulsion has reached zero;
- controlled braking has reached its maximum simulation value;
- vehicle may still be moving because physical speed does not instantly become zero.

The speed feedback continues to report movement until the dynamic model reaches the stop threshold.

Only then should the simulated lockout condition be considered physically complete.
