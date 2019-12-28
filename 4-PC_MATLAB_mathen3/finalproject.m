%% finds if comport is not been closed and deleted and does so if it has been not
old = instrfind;      
if(~isempty(old))   
    fclose(old);
    delete(old);
end
clear;

%%set serial port
s = serial('COM4','BaudRate',19200,'Terminator','CR'); 
fopen(s);

%%axis variables 
time = now;
theta = 0;

%figure
display = figure('Name', 'Time v. Theta');

% set axes and ranges
axesHandle = axes('Parent',display,'Color',[1 1 1]);
hold on;
ylim([0 90]);

% titles and labels
title('Time v. Theta','FontWeight','bold','Color','k');
xlabel('Time','Color','k');
ylabel('Theta','Color','k');

% plot data on graph
count = 1;
while true
    time(count) = now; 
    theta(count) = fread(s,1,'uchar');
    count = count + 1;
    plot(axesHandle,time,theta,'Color',[0.8 0.5 0]);
    datetick('x','SS');
    pause(0.01);
end
