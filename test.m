%connect to the serialport of the stm32 and set up arrays to hold the
%values read between T = 0-10s
device = serialport("/dev/cu.usbmodem1203", 115200);
configureTerminator(device, "CR/LF");
flush(device);


T = 10;          
t0 = tic;

t = [];
ax = []; ay = []; az = [];
gx = []; gy = []; gz = [];

while toc(t0) < T
    line = readline(device);
    vals = sscanf(line, "%d");

    if numel(vals) ~= 7
        continue;
    end

    ax_r = vals(1); ay_r = vals(2); az_r = vals(3);
    gx_r = vals(4); gy_r = vals(5); gz_r = vals(6);

    ax(end+1) = ax_r / 16384;
    ay(end+1) = ay_r / 16384;
    az(end+1) = az_r / 16384;

    gx(end+1) = gx_r / 131;
    gy(end+1) = gy_r / 131;
    gz(end+1) = gz_r / 131;

    t(end+1) = toc(t0);
end

% close serial port
clear device 

%figure for accel
figure;
plot(t, ax, t, ay, t, az);
grid on;
xlabel("Time (s)");
ylabel("Acceleration (g)");
legend("ax","ay","az");
title("MPU6050 Accelerometer");

%figure for the gyro
figure;
plot(t, gx, t, gy, t, gz);
grid on;
xlabel("Time (s)");
ylabel("Angular rate (deg/s)");
legend("gx","gy","gz");
title("MPU6050 Gyroscope");
